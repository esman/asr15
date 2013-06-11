#include "watchdog.h"

#include "stm32f10x.h"
#include "stm32f10x_conf.h"

#define WATCHDOG_TIMEOUT 10000 //max = 4095

void InitWatchdog()
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_4);
	IWDG_SetReload(WATCHDOG_TIMEOUT);
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Disable);
	IWDG_Enable();
}

void ResetWatchdog()
{
	IWDG_ReloadCounter();
}
