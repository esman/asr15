#include <stdio.h>

#include "generic.h"
#include "menu.h"
#include "algorithm.h"
#include "../periph/io.h"
#include "../periph/button.h"

#include "../periph/display.h"

void Init()
{
	ButtonInit();
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
