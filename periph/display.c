#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

#include "display.h"
#include "../gpio.h"
#include "../krnl/algorithm.h"

#define LCD_ROWS 64
#define LCD_COLUMNS 132

#define LCD_RESET_PORT GPIOB
#define LCD_RESET_PIN 9

#define LCD_I2C_PORT I2C1

#define LCD_I2C_SCL_PORT GPIOB
#define LCD_I2C_SCL_PIN 6
#define LCD_I2C_SDA_PORT GPIOB
#define LCD_I2C_SDA_PIN 7

#define LCD_RESET_OFF GPIO_SET(LCD_RESET_PORT, LCD_RESET_PIN)
#define LCD_RESET_ON GPIO_RESET(LCD_RESET_PORT, LCD_RESET_PIN)

#define LCD_TIMEOUT 1000
#define LCD_STOP_TIMEOUT 50

#define LCD_COMMAND_ADDRESS 0x70
#define LCD_DATA_ADDRESS 0x72

#define I2C_TRANSMIT 1
#define I2C_RECEIVE 0

#define I2C_DMA_CHANNEL    DMA1_Channel6
#define I2C_DMA_ISR_TCIF   DMA_ISR_TCIF6
#define I2C_DMA_IFCR_CTCIF DMA_IFCR_CTCIF6

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
  0xC6, // To rotate screen by 180 deg set 0xC6. 0 deg - 0xC0
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
  // Configure SCL and SDA as open-drain outputs
  uint32_t reg = GPIOB->CRL;
  reg &= ~(GPIO_CRL_CNF6_1 | GPIO_CRL_CNF7_1);
  GPIOB->CRL = reg
  | GPIO_CRL_CNF6_0 | GPIO_CRL_MODE6_0 | GPIO_CRL_MODE6_1
  | GPIO_CRL_CNF7_0 | GPIO_CRL_MODE7_0 | GPIO_CRL_MODE7_1
  ;

  // Reset line
  GPIOB->BRR = UINT32_BIT(LCD_I2C_SCL_PIN) | UINT32_BIT(LCD_I2C_SDA_PIN);
}

void I2C_Init_(void)
{
  I2C_DeInit(LCD_I2C_PORT);

  // Check i2c line is idle
  uint32_t reg = GPIOB->IDR;
  if(!(reg & UINT32_BIT(LCD_I2C_SDA_PIN)) && (reg & UINT32_BIT(LCD_I2C_SCL_PIN)))
  {
    LcdResetLine();
  }

  /* Configure i2c pins */
  GPIOB->CRL |= (uint32_t) 0
  // SCL - AF OD
  | GPIO_CRL_CNF6_0 | GPIO_CRL_CNF6_1 | GPIO_CRL_MODE6_0 | GPIO_CRL_MODE6_1
  // SDA - AF OD
  | GPIO_CRL_CNF7_0 | GPIO_CRL_CNF7_1 | GPIO_CRL_MODE7_0 | GPIO_CRL_MODE7_1
  ;

  // Enable event and error interrupts and DMA requests
  LCD_I2C_PORT->CR2 |= I2C_CR2_ITEVTEN | I2C_CR2_ITERREN | I2C_CR2_DMAEN;

  I2C_InitTypeDef  I2C_InitStructure;
  I2C_StructInit(&I2C_InitStructure);
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
  LCD_RESET_ON;
  DELAY(16000);
  LCD_RESET_OFF;
  DELAY(16000);

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

  LCD_RESET_ON;
  LCD_RESET_OFF;

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

    // read SR2 register just to clear ADDR flag
    (void) LCD_I2C_PORT->SR2;
  }
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

void I2C1_EV_IRQHandler()
{
  lcdI2CFunc();
}

void I2C1_ER_IRQHandler()
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
