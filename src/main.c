#include <string.h>
#include "utils.h"
#include "periph/fram.h"
#include "periph/display.h"
#include "krnl/menu.h"
#include "krnl/algorithm.h"

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
  | GPIO_CRL_MODE4_0 | GPIO_CRL_MODE4_1 // FRAM_HOLD - OUT Push-pull 50Mhz
  | GPIO_CRL_MODE5_0 | GPIO_CRL_MODE5_1 // FRAM_SCK - OUT Push-pull 50Mhz
  | GPIO_CRL_CNF6_1 // FRAM_SO - Input pull-down
  | GPIO_CRL_MODE7_0 | GPIO_CRL_MODE7_1 // FRAM_SI - OUT Push-pull 50MHz
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

  GPIOA->BSRR = (uint32_t) 0
  | UINT32_BIT(4)  // FRAM_HOLD
  | UINT32_BIT(8)  // BUTTON_UP
  | UINT32_BIT(12) // BUTTON_RIGHT
  ;

  GPIOB->BSRR = (uint32_t) 0
  | UINT32_BIT(1)  // FRAM_CS
  | UINT32_BIT(14) // LCD_LED
  ;

  GPIOC->BSRR = (uint32_t) 0
  | UINT32_BIT(13) // Button 'DOWN' - Input pull-up
  | UINT32_BIT(14) // Button 'LEFT' - Input pull-up
  | UINT32_BIT(15) // Button 'OK' - Input pull-up
  ;

  /* FRAM initialization */
  FramInit();
  FramRead(0, &algo_presets, sizeof(algo_presets_t));

  LcdInit();
  MenuInit();

  /* IWDG configuration */
  IWDG->KR  = 0x5555; // enable write access
  IWDG->PR  = 1;      // !!!zero prescaler disables iwdg
  IWDG->RLR = 300;    // timeout (empiric value)
  IWDG->KR  = 0xCCCC; // enable watchdog

  /* NVIC configuration */
  NVIC->ISER[0] = UINT32_BIT(I2C1_EV_IRQn % 32);
  NVIC->ISER[1] = UINT32_BIT(I2C1_ER_IRQn % 32);
  NVIC->IP[I2C1_EV_IRQn] = 0xFF;
  NVIC->IP[I2C1_ER_IRQn] = 0xFF;

  algo_presets_t presets = algo_presets;

  while(1)
  {
    IWDG->KR  = 0xAAAA; // Reload watchdog counter
    AlgoMain();
    MenuProc();
    LcdCheck();

    while(memcmp(&presets, &algo_presets, sizeof(algo_presets_t)))
    {
      FramWrite(0, &algo_presets, sizeof(algo_presets_t));
      FramRead(0, &presets, sizeof(algo_presets_t));
    }
  }

  return 0;
}
