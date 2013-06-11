#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

#include "display.h"
#include "utils.h"
#include "../krnl/algorithm.h"

#include "stm32f10x.h"

#define LCD_ROWS 64
#define LCD_COLUMNS 132

#define LCD_LED_PORT GPIOB
#define LCD_LED_PIN GPIO_Pin_3

#define LCD_RESET_PORT GPIOB
#define LCD_RESET_PIN GPIO_Pin_2

#define LCD_I2C_PORT I2C2

#define LCD_I2C_SCL_PORT GPIOB
#define LCD_I2C_SCL_PIN GPIO_Pin_10
#define LCD_I2C_SDA_PORT GPIOB
#define LCD_I2C_SDA_PIN GPIO_Pin_11

#define RSETLCD_OFF GPIO_SetBits(LCD_RESET_PORT, LCD_RESET_PIN);
#define RSETLCD_ON GPIO_ResetBits(LCD_RESET_PORT, LCD_RESET_PIN);

#define LCD_TIMEOUT 1000
#define LCD_STOP_TIMEOUT 50

#define LCD_COMMAND_ADDRESS 0x70
#define LCD_DATA_ADDRESS 0x72

#define I2C_TRANSMIT 1
#define I2C_RECEIVE 0

#define I2C_DMA_CHANNEL    DMA1_Channel4
#define I2C_DMA_ISR_TCIF   DMA_ISR_TCIF4
#define I2C_DMA_IFCR_CTCIF DMA_IFCR_CTCIF4

static uint8_t const LCD_CONFIG[] =
{
	// Set column address
	0x00, // LSB (0)
	0x10, // MSB (0)
	// Set temperature compensation
	0x24, // 0
	// Set power control
	0x2E,
	// Set advanced program control
	0x30, // disable
	// Set scroll line
	0x40, // 0
	// Set page address
	0xB0, // 0
	// Set Vbias potentiometer
	0x81, // command
	0x78, // value = 120
	// Set partial display control
	0x84, // disable
	// Set RAM address control
	0x89, // automatic page wrap around
	// Set frame rate
	0xA0, // 80 fps
	// Set all pixels on
	0xA4, // disable
	// Set LCD mapping control
	0xC0, // To rotate screen by 180 deg set 0xC6
	// Set test control
	0xE4, // TT = 00
	0x00, // testing parameter 0
	// Set LCD BIAS ratio
	0xEB, // 6
	// Set COM End
	0xF1, //command
	0x3f, // 64 - 1
	// Set partial display start
	0xF2, // command
	0x00, // 0
	// Set partial display end
	0xF3, // command
	0x3f, // 63
	// Set display enable
	0xAF, // enable
};

static char lcdPixMap[LCD_ROWS * LCD_COLUMNS / 8];

void (*lcdFunc)();
void (*lcdI2CFunc)();
uint8_t lcdAddr;

int lcdTimeout;

void LcdSendConfig();
void LcdSendData();
void LcdSystemReset();

void LcdOnSB();
void LcdOnADDR();
void LcdOnBTF();

int lcdChanged;

void LcdResetLine()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_InitStructure.GPIO_Pin  = LCD_I2C_SDA_PIN;
	GPIO_Init(LCD_I2C_SDA_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin  = LCD_I2C_SCL_PIN;
	GPIO_Init(LCD_I2C_SCL_PORT, &GPIO_InitStructure);

	GPIO_WriteBit(LCD_I2C_SCL_PORT, LCD_I2C_SCL_PIN, Bit_RESET);
	GPIO_WriteBit(LCD_I2C_SDA_PORT, LCD_I2C_SDA_PIN, Bit_RESET);
//	Delay(1000);
//	GPIO_WriteBit(LCD_I2C_SCL_PORT, LCD_I2C_SCL_PIN, Bit_RESET);
//	GPIO_WriteBit(LCD_I2C_SDA_PORT, LCD_I2C_SDA_PIN, Bit_SET);
}

void I2C_Init_(void)
{
	I2C_InitTypeDef  I2C_InitStructure;
	I2C_DeInit(LCD_I2C_PORT);
	
	int sda = GPIO_ReadInputDataBit(LCD_I2C_SDA_PORT, LCD_I2C_SDA_PIN);
	int scl = GPIO_ReadInputDataBit(LCD_I2C_SCL_PORT, LCD_I2C_SCL_PIN);
	
	if(scl && !sda)
	{
		LcdResetLine();
	}

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_InitStructure.GPIO_Pin = LCD_I2C_SDA_PIN;
	GPIO_Init(LCD_I2C_SDA_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin  = LCD_I2C_SCL_PIN;
	GPIO_Init(LCD_I2C_SCL_PORT, &GPIO_InitStructure);

	I2C_StructInit(&I2C_InitStructure);
	
	// Enable event and error interrupts
	LCD_I2C_PORT->CR2 |= I2C_CR2_ITEVTEN;
	LCD_I2C_PORT->CR2 |= I2C_CR2_ITERREN;
	
	// Enable DMA requests
	LCD_I2C_PORT->CR2 |= I2C_CR2_DMAEN;
	
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_ClockSpeed = 400000;  /* 400kHz */

	I2C_Init(LCD_I2C_PORT, &I2C_InitStructure);
}

void I2C_SendDMA(uint8_t addr, void const* buff, uint16_t size)
{
	I2C_Init_();

	// configure memory address and size
	I2C_DMA_CHANNEL->CCR  &= ~DMA_CCR4_EN;
	I2C_DMA_CHANNEL->CMAR  = (uint32_t) buff;
	I2C_DMA_CHANNEL->CNDTR =  size;
	I2C_DMA_CHANNEL->CCR  |=  DMA_CCR4_EN;

	lcdI2CFunc = LcdOnSB;
	lcdAddr = addr;
	
	// generate START
	LCD_I2C_PORT->CR1 |= I2C_CR1_START;
}

void LcdInit()
{
	I2C_DMA_CHANNEL->CPAR = (uint32_t) &LCD_I2C_PORT->DR;
	I2C_DMA_CHANNEL->CCR = DMA_CCR4_MINC | DMA_CCR4_DIR | DMA_CCR4_TEIE | DMA_CCR4_TCIE;

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_InitStructure.GPIO_Pin = LCD_LED_PIN;
	GPIO_Init(LCD_LED_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = LCD_RESET_PIN;
	GPIO_Init(LCD_RESET_PORT, &GPIO_InitStructure);
	
	LcdLight(1);
	
	RSETLCD_ON;
	Delay(16000);
	RSETLCD_OFF;
	Delay(16000);

	LcdSystemReset();
}

void LcdSet(int row, int column, int state)
{
    assert(row >= 0 && row < LCD_ROWS && column >= 0 && column < LCD_COLUMNS);
	int byte = column + (row / 8) * LCD_COLUMNS;
	int bit = row % 8;
	if(state)
	{
		lcdPixMap[byte] |= (1 << bit);
	}
	else
	{
		lcdPixMap[byte] &= ~(1 << bit);
	}
}

void LcdLight(int state)
{
	GPIO_WriteBit(LCD_LED_PORT, LCD_LED_PIN, state ? Bit_SET : Bit_RESET);
}

void LcdSendConfig()
{
	lcdFunc = LcdSendData;
	I2C_SendDMA(LCD_COMMAND_ADDRESS, LCD_CONFIG, sizeof(LCD_CONFIG));
}

void LcdSendData()
{
	lcdFunc = LcdSendConfig;
	I2C_SendDMA(LCD_DATA_ADDRESS, lcdPixMap, sizeof(lcdPixMap));
}

void LcdSystemReset()
{
	static uint8_t const reset = 0xE2;
	
	RSETLCD_ON;
//	Delay(16000);
	RSETLCD_OFF;

	lcdFunc = LcdSendConfig;
	I2C_SendDMA(LCD_DATA_ADDRESS, &reset, 1);
}

void LcdOnSB()
{
	if(LCD_I2C_PORT->SR1 & I2C_SR1_SB)
	{
		lcdI2CFunc = LcdOnADDR;
		LCD_I2C_PORT->DR = lcdAddr;
	}
	else
	{
		LcdSystemReset();
	}
}

void LcdOnADDR()
{
	if(LCD_I2C_PORT->SR1 & I2C_SR1_ADDR)
	{
		lcdI2CFunc = LcdOnBTF;

		// read SR2 register just for clear ADDR flag
		(void) LCD_I2C_PORT->SR2;
	}
	// else
	// {
		// LcdSystemReset();
	// }
}

void LcdOnBTF()
{
	int timeout = 0;
	
	// generate STOP
	LCD_I2C_PORT->CR1 |= I2C_CR1_STOP;
	
	// wait for STOP = 0
	while(LCD_I2C_PORT->CR1 & I2C_CR1_STOP)
	{
		if(++timeout > LCD_STOP_TIMEOUT)
		{
			LcdSystemReset();
			return;
		}
	}

	lcdTimeout = 0;
	lcdFunc();
}

void I2C2_EV_IRQHandler()
{
	lcdI2CFunc();
}

void I2C2_ER_IRQHandler()
{
	LcdSystemReset();
}

void LcdCheck()
{
	if(++lcdTimeout > LCD_TIMEOUT)
	{
		lcdTimeout = 0;
		LcdSystemReset();
	}
}
