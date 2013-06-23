#include <string.h>
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "fram.h"
#include "utils.h"
#include "periph/utils.h"
#include "periph/watchdog.h"
#include "periph/display.h"
#include "krnl/menu.h"
#include "krnl/algorithm.h"

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
  | RCC_APB2ENR_ADC1EN // enable ADC1
  ;

  /* DMA configuration */
  DMA1_Channel6->CPAR = (uint32_t) &I2C1->DR;
  DMA1_Channel6->CCR  = DMA_CCR1_MINC | DMA_CCR1_DIR | DMA_CCR1_TEIE | DMA_CCR1_TCIE;  // NVIC configuration

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

  GPIOA->CRH = (uint32_t) 0
  | GPIO_CRH_CNF8_1 // Button 'UP' - Input pull-up
  | GPIO_CRH_CNF9_0 // Default - Input floating
  | GPIO_CRH_CNF10_0 // Default - Input floating
  | GPIO_CRH_CNF11_0 // Default - Input floating
  | GPIO_CRH_CNF12_1 // Button 'RIGHT' - Input pull-up
  | GPIO_CRH_CNF13_0 // Default - Input floating
  | GPIO_CRH_CNF14_0 // Default - Input floating
  | GPIO_CRH_CNF15_0 // Default - Input floating
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

  GPIOB->CRH = (uint32_t) 0
  | GPIO_CRH_CNF8_0 // Default - Input floating
  | GPIO_CRH_MODE9_0 | GPIO_CRH_MODE9_1 // LCD_RESET - OUT Push-pull 50MHz
  | GPIO_CRH_CNF10_0 // Default - Input floating
  | GPIO_CRH_CNF11_0 // Default - Input floating
  | GPIO_CRH_MODE12_0 | GPIO_CRH_MODE13_1 // OUT1 - OUT Push-pull 50MHz
  | GPIO_CRH_MODE12_0 | GPIO_CRH_MODE13_1 // OUT2 - OUT Push-pull 50MHz
  | GPIO_CRH_MODE14_0 | GPIO_CRH_MODE14_1 // LCD_LED - OUT Push-pull 50MHz
  | GPIO_CRH_CNF15_0 // Default - Input floating
  ;

  GPIOC->CRH = (uint32_t) 0
  | GPIO_CRH_CNF8_0 // Default - Input floating
  | GPIO_CRH_CNF9_0 // Default - Input floating
  | GPIO_CRH_CNF10_0 // Default - Input floating
  | GPIO_CRH_CNF11_0 // Default - Input floating
  | GPIO_CRH_CNF12_0 // Default - Input floating
  | GPIO_CRH_CNF13_1 // Button 'DOWN' - Input pull-up
  | GPIO_CRH_CNF14_1 // Button 'LEFT' - Input pull-up
  | GPIO_CRH_CNF15_1 // Button 'OK' - Input pull-up
  ;

  GPIOA->BSRR = UINT32_BIT(8) | UINT32_BIT(12); // BUTTON_UP, BUTTON_RIGHT
  GPIOB->BSRR = UINT32_BIT(1) | UINT32_BIT(14); // FRAM_CS, LCD_LED
  GPIOC->BSRR = (uint32_t) 0
  | UINT32_BIT(13) // Button 'DOWN' - Input pull-up
  | UINT32_BIT(14) // Button 'LEFT' - Input pull-up
  | UINT32_BIT(15) // Button 'OK' - Input pull-up
  ;

  FramInit();
  FramRead(0, &algo_presets, sizeof(algo_presets_t));

  LcdInit();
  MenuInit();
  WatchdogInit();

  /* NVIC configuration */
  NVIC->ISER[0] = UINT32_BIT(I2C1_EV_IRQn % 32);
  NVIC->ISER[1] = UINT32_BIT(I2C1_ER_IRQn % 32);
  NVIC->IP[I2C1_EV_IRQn] = 0xFF;
  NVIC->IP[I2C1_ER_IRQn] = 0xFF;

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
    WatchdogReset();
    AlgoMain();
    MenuProc();
    LcdCheck();

    while(!(ADC1->SR & ADC_SR_EOC));

    if(ADC1->DR >= POWER_THRESHOLD)
    {
      power_ok = 1;
    }
    else if(power_ok)
    {
      power_ok = 0;

      algo_presets_t data_read;
      do
      {
        FramWrite(0, &algo_presets, sizeof(algo_presets_t));
        FramRead(0, &data_read, sizeof(algo_presets_t));
      } while(memcmp(&algo_presets, &data_read, sizeof(algo_presets_t)));
    }
  }

  return 0;
}
