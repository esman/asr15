#include <string.h>
#include "utils.h"
#include "periph/fram.h"
#include "krnl/menu.h"
#include "krnl/algorithm.h"

void LedProc(void);

int main()
{
  RCC->APB2ENR = (uint32_t) 0
  | RCC_APB2ENR_AFIOEN // enable alternative functions
  | RCC_APB2ENR_IOPAEN // enable GPIO port A
  | RCC_APB2ENR_IOPBEN // enable GPIO port B
  | RCC_APB2ENR_IOPCEN // enable GPIO port C
  ;

  /* GPIO configuration */
  GPIOA->CRL = (uint32_t) 0
  | GPIO_CRL_CNF0_0 // Default - Input floating
  | GPIO_CRL_CNF1_0 // Default - Input floating
  | GPIO_CRL_CNF2_0 // Default - Input floating
  | GPIO_CRL_MODE3_0 | GPIO_CRL_MODE3_1 // FRAM_HOLD - OUT Push-pull 50Mhz
  | GPIO_CRL_CNF4_0 // Default - Input floating
  | GPIO_CRL_MODE5_0 | GPIO_CRL_MODE5_1 // FRAM_SCK - OUT Push-pull 50Mhz
  | GPIO_CRL_CNF6_1 // FRAM_SO - Input pull-down
  | GPIO_CRL_MODE7_0 | GPIO_CRL_MODE7_1 // FRAM_SI - OUT Push-pull 50MHz
  ;

  GPIOA->CRH = (uint32_t) 0
  | GPIO_CRH_MODE8_0 | GPIO_CRH_MODE8_1 // LED DP - OUT Push-pull 50MHz
  | GPIO_CRH_MODE9_0 | GPIO_CRH_MODE9_1 // LED DGT_4 - OUT Push-pull 50MHz
  | GPIO_CRH_MODE10_0 | GPIO_CRH_MODE10_1 // LED DGT_3 - OUT Push-pull 50MHz
  | GPIO_CRH_MODE11_0 | GPIO_CRH_MODE11_1 // LED DGT_2 - OUT Push-pull 50MHz
  | GPIO_CRH_MODE12_0 | GPIO_CRH_MODE12_1 // LED DGT_1 - OUT Push-pull 50MHz
  | GPIO_CRH_CNF13_0 // Default - Input floating
  | GPIO_CRH_CNF14_0 // Default - Input floating
  | GPIO_CRH_MODE15_0 | GPIO_CRH_MODE15_1 // LED2 - OUT Push-pull 50MHz
  ;

  GPIOB->CRL = (uint32_t) 0
  | GPIO_CRL_MODE0_0 | GPIO_CRL_MODE0_1 // FRAM_CS - OUT Push-pull 50Mhz
  | GPIO_CRL_MODE1_0 | GPIO_CRL_MODE1_1 // FRAM_WP - OUT Push-pull 50Mhz
  | GPIO_CRL_MODE2_0 | GPIO_CRL_MODE2_1 // LED SEG_B - OUT Push-pull 50MHz
  | GPIO_CRL_MODE3_0 | GPIO_CRL_MODE3_1 // LED1 - OUT Push-pull 50MHz
  | GPIO_CRL_MODE4_0 | GPIO_CRL_MODE4_1 // OUT1 - OUT Push-pull 50MHz
  | GPIO_CRL_MODE5_0 | GPIO_CRL_MODE5_1 // OUT2 - OUT Push-pull 50MHz
  | GPIO_CRL_CNF6_1 // Button 'MODE' - Input pull-up
  | GPIO_CRL_CNF7_1 // Button 'SET' - Input pull-up
  ;

  GPIOB->CRH = (uint32_t) 0
  | GPIO_CRH_CNF8_0 // IN2 - Input floating
  | GPIO_CRH_CNF9_0 // IN3 - Input floating
  | GPIO_CRH_MODE10_0 | GPIO_CRH_MODE10_1 // LED SEG_F - OUT Push-pull 50MHz
  | GPIO_CRH_MODE11_0 | GPIO_CRH_MODE11_1 // LED SEG_A - OUT Push-pull 50MHz
  | GPIO_CRH_MODE12_0 | GPIO_CRH_MODE12_1 // LED SEG_G - OUT Push-pull 50MHz
  | GPIO_CRH_MODE13_0 | GPIO_CRH_MODE13_1 // LED SEG_C - OUT Push-pull 50MHz
  | GPIO_CRH_MODE14_0 | GPIO_CRH_MODE14_1 // LED SEG_D - OUT Push-pull 50MHz
  | GPIO_CRH_MODE15_0 | GPIO_CRH_MODE15_1 // LED SEG_E - OUT Push-pull 50MHz
  ;

  GPIOC->CRH = (uint32_t) 0
  | GPIO_CRH_CNF8_0 // Default - Input floating
  | GPIO_CRH_CNF9_0 // Default - Input floating
  | GPIO_CRH_CNF10_0 // Default - Input floating
  | GPIO_CRH_CNF11_0 // Default - Input floating
  | GPIO_CRH_CNF12_0 // Default - Input floating
  | GPIO_CRH_CNF13_0 // IN1 - Input floating
  | GPIO_CRH_CNF14_0 // Default - Input floating
  | GPIO_CRH_CNF15_0 // Default - Input floating
  ;

  GPIOA->BSRR = (uint32_t) 0
  | UINT32_BIT(3)  // FRAM_HOLD
  ;

  GPIOB->BSRR = (uint32_t) 0
  | UINT32_BIT(6)  // BUTTON MODE
  | UINT32_BIT(7)  // BUTTON SET
  | UINT32_BIT(0)  // FRAM_CS
  ;

  AFIO->MAPR |= (uint32_t) 0
    | AFIO_MAPR_SWJ_CFG_JTAGDISABLE
  ;

  // Setup sys timer with period of 1ms
  SysTick_Config(8000);

  /* FRAM initialization */
  FramInit();
  FramRead(0, &algo_presets, sizeof(algo_presets_t));

  /* IWDG configuration */
  IWDG->KR  = 0x5555; // enable write access
  IWDG->PR  = 1;      // !!!zero prescaler disables iwdg
  IWDG->RLR = 300;    // timeout (empiric value)
  IWDG->KR  = 0xCCCC; // enable watchdog

  algo_presets_t presets = algo_presets;

  while(1)
  {
    IWDG->KR  = 0xAAAA; // Reload watchdog counter
    AlgoMain();
    MenuProc();
    LedProc();

    while(memcmp(&presets, &algo_presets, sizeof(algo_presets_t)))
    {
      FramWrite(0, &algo_presets, sizeof(algo_presets_t));
      FramRead(0, &presets, sizeof(algo_presets_t));
    }
  }

  return 0;
}
