#include <stdio.h>

#include "generic.h"
#include "menu.h"
#include "algorithm.h"
#include "../periph/io.h"

#include "../periph/display.h"

void Init()
{
//	InitIO();
	LcdInit();
	MenuInit();
}

void MainLoop()
{
//	AlgoMain();
	MenuProc();
	LcdCheck();
}
