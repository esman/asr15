#include <stdio.h>

#include "generic.h"
#include "menu.h"
#include "algorithm.h"
#include "../periph/io.h"
#include "../periph/button.h"
#include "../periph/watchdog.h"

#include "../periph/display.h"
#include "../periph/backup.h"

void Init()
{
	ButtonInit();
	InitIO();
	LcdInit();
	BcpInit();
	AlgoInit();
	MenuInit();
	InitWatchdog();
}

void MainLoop()
{
	AlgoMain();
	MenuProc();
	LcdCheck();
	ResetWatchdog();
}
