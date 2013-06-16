#include <stdio.h>

#include "generic.h"
#include "menu.h"
#include "algorithm.h"
#include "../periph/io.h"
#include "../periph/display.h"

void Init()
{
	LcdInit();
	MenuInit();
}

void MainLoop()
{
	AlgoMain();
	MenuProc();
	LcdCheck();
}
