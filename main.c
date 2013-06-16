#include <string.h>
#include "krnl/generic.h"
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "fram.h"
#include "utils.h"
#include "periph/utils.h"
#include "periph/watchdog.h"

extern int lineCounter;
extern int linePreset;
extern int spirePreset;
extern int algoDelay;
extern int lineTotal;
extern int readyTotal;

typedef struct
{
  int lineCounter;
  int linePreset;
  int spirePreset;
  int algoDelay;
  int lineTotal;
  int readyTotal;
} persistant_t;

void IniAdc (void)
{
  ADC_InitTypeDef ADC_InitStructure;
  /* ADC1 configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);
  /* ADC1 regular channel14 configuration */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1,ADC_SampleTime_1Cycles5);
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);
  /* Enable ADC1 reset calibration register */   
  ADC_ResetCalibration(ADC1);
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));
  /* Start ADC1 calibration */
  ADC_StartCalibration(ADC1);
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));     
  /* Start ADC1 Software Conversion */ 
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

#define POWER_THRESHOLD ((uint16_t) 0x500)

int main()
{
  /* RCC configuration */
  RCC->CFGR    = RCC_CFGR_ADCPRE_DIV2; // ADC frequency prescaler
  RCC->AHBENR  = RCC_AHBENR_DMA1EN;    // enable DMA1
  RCC->APB1ENR = RCC_APB1ENR_I2C1EN;   // enable I2C1

  RCC->APB2ENR = (uint32_t) 0
  | RCC_APB2ENR_AFIOEN // enable alternative functions
  | RCC_APB2ENR_IOPAEN // enable GPIO port A
  | RCC_APB2ENR_IOPBEN // enable GPIO port B
  | RCC_APB2ENR_IOPCEN // enable GPIO port C
  | RCC_APB2ENR_IOPDEN // enable GPIO port D
  | RCC_APB2ENR_ADC1EN // enable ADC1
  ;
  
  // NVIC configuration
  
  NVIC_InitTypeDef NVIC_InitStructure;
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  //select NVIC channel to configure
  NVIC_InitStructure.NVIC_IRQChannel = I2C1_EV_IRQn;
   
  //set priority to lowest
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
   
  //set subpriority to lowest
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
   
  //enable IRQ channel
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   
  //update NVIC registers
  NVIC_Init(&NVIC_InitStructure);
  
  //select NVIC channel to configure
  NVIC_InitStructure.NVIC_IRQChannel = I2C1_ER_IRQn;
   
  //update NVIC registers
  NVIC_Init(&NVIC_InitStructure);

  /* GPIO configuration */
  GPIOA->CRL = (uint32_t) 0
  | GPIO_CRL_CNF0_0 // Default - Input floating
  | GPIO_CRL_CNF1_0 // Button 'RIGHT' - Input floating
  | GPIO_CRL_CNF2_0 // IN2 - Input floating
  | GPIO_CRL_CNF3_0 // IN1 - Input floating
  | GPIO_CRL_MODE4_0 | GPIO_CRL_MODE4_1 // SPI1_NSS - OUT Push-pull 50Mhz
  | GPIO_CRL_MODE5_0 | GPIO_CRL_MODE5_1 // SPI1_SCK - OUT Push-pull 50Mhz
  | GPIO_CRL_CNF6_1 // SPI1_MISO - Input pull-down
  | GPIO_CRL_MODE7_0 | GPIO_CRL_MODE7_1 // SPI_MOSI - OUT Push-pull 50MHz
  ;

  GPIOB->CRL = (uint32_t) 0
  | GPIO_CRL_MODE0_0 | GPIO_CRL_MODE0_1 // FRAM_WP - OUT Push-pull 50Mhz
  | GPIO_CRL_MODE1_0 | GPIO_CRL_MODE1_1 // FRAM_CS - OUT Push-pull 50Mhz
  | GPIO_CRL_CNF2_0 // Default - Input floating
  | GPIO_CRL_CNF3_0 // Default - Input floating
  | GPIO_CRL_CNF4_0 // Default - Input floating
  | GPIO_CRL_CNF5_0 // Default - Input floating
  | GPIO_CRL_CNF6_0 // Default - Input floating
  | GPIO_CRL_CNF7_0 // Default - Input floating
  ;

  GPIOB->BSRR = UINT32_BIT(1); // FRAM_CS
  
 	InitWatchdog();

  /* Check FRAM is ready */
  uint32_t passphrase;
  do
  {
    passphrase = 0xDEADBEEF;
    FramWrite(500, &passphrase, sizeof(uint32_t));
    FramRead (500, &passphrase, sizeof(uint32_t));
  } while(passphrase != 0xDEADBEEF);
  
  persistant_t data_read;
  persistant_t data_write;
  FramRead(0, &data_read, sizeof(persistant_t));

  lineCounter = data_read.lineCounter;
  linePreset = data_read.linePreset;
  spirePreset = data_read.spirePreset;
  algoDelay = data_read.algoDelay;
  lineTotal = data_read.lineTotal;
  readyTotal = data_read.readyTotal;

  Init();
  
  GPIO_InitTypeDef GPIO_InitStructure;
  memset(&GPIO_InitStructure, 0, sizeof(GPIO_InitStructure));
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; 
  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  IniAdc();
  
  int power_ok = 0;
  
  while(1)
  {
    ResetWatchdog();
    MainLoop();

    while(!(ADC1->SR & ADC_SR_EOC));

    if(ADC1->DR >= POWER_THRESHOLD)
    {
      power_ok = 1;
    }
    else if(power_ok)
    {
      break;
    }
  }

  data_write.lineCounter = lineCounter;
  data_write.linePreset = linePreset;
  data_write.spirePreset = spirePreset;
  data_write.algoDelay = algoDelay;
  data_write.lineTotal = lineTotal;
  data_write.readyTotal = readyTotal;

  do
  {
    FramWrite(0, &data_write, sizeof(persistant_t));
    FramRead(0, &data_read, sizeof(persistant_t));
  } while(memcmp(&data_write, &data_read, sizeof(persistant_t)));
}
