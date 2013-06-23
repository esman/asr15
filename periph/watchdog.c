#include "watchdog.h"

#include "stm32f10x.h"

void InitWatchdog()
{
  IWDG->KR  = 0x5555; // enable write access
  IWDG->PR  = 1;      // !!!zero prescaler disables iwdg
  IWDG->RLR = 300;    // timeout (empiric value)
  IWDG->KR  = 0xCCCC; // enable watchdog
}

void ResetWatchdog()
{
  IWDG->KR  = 0xAAAA;
}
