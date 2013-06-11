#include "algorithm.h"
#include "generic.h"
#include "../periph/io.h"
#include "../periph/backup.h"
#include "../periph/utils.h"
#include "../periph/display.h"

#define ALGO_RESET_INDEX 7
#define ALGO_PLUS1_INDEX 6
#define ALGO_COUNT_INDEX 5

#define ALGO_RUN_INDEX 2
#define ALGO_CUT_INDEX 3

#define ALGO_DELAY_SCALER 10
#define ALGO_DELAY (algoDelay * ALGO_DELAY_SCALER)

#define ALGO_LINE_REGISTER 0
// #define ALGO_SPIRE_REGISTER 1
#define ALGO_SPIRE_PRESET_REGISTER 2
#define ALGO_LINE_PRESET_REGISTER 3
#define ALGO_DELAY_REGISTER 4
#define ALGO_TOTAL_LINES_REGISTER 5
#define ALGO_TOTAL_READY_REGISTER 6

#define ALGO_MIN_SPIRES 5

int spirePreset;
int linePreset;
int algoDelay;
int algoDelayCounter;

int lineCounter;
int spireCounter;

int lineTotal;
int spireTotal;
int readyTotal;

int reset;
int plus1;
int count;

int cut;
int run;

void AlgoCountSpires();
void WaitForReset();
void AlgoDelay();

void (*AlgoFunc)() = AlgoCountSpires;

void AlgoInit()
{
	lineCounter = BcpRead(ALGO_LINE_REGISTER);
	// spireCounter = BcpRead(ALGO_SPIRE_REGISTER);
	linePreset = BcpRead(ALGO_LINE_PRESET_REGISTER);
	spirePreset = BcpRead(ALGO_SPIRE_PRESET_REGISTER);
	algoDelay = BcpRead(ALGO_DELAY_REGISTER);
	lineTotal = BcpRead(ALGO_TOTAL_LINES_REGISTER);
	readyTotal = BcpRead(ALGO_TOTAL_READY_REGISTER);
}

void AlgoMain()
{
	static int prevCount;
	if(GetIN(ALGO_COUNT_INDEX))
	{
		count = prevCount ? 0 : 1;
		prevCount = 1;
	}
	else
	{
		prevCount = 0;
		count = 0;
	}
	reset = GetIN(ALGO_RESET_INDEX);
	plus1 = GetIN(ALGO_PLUS1_INDEX);

	if(reset)
	{
		spireCounter = 0;
		cut = 0;
	}
	
	
	// Run control
	if(count)
	{
		run = 0;
	}
	else if(algoDelayCounter)
	{
		if(!(--algoDelayCounter))
		{
			run = 1;
		}
	}

	if(spirePreset >= ALGO_MIN_SPIRES) AlgoFunc();

	SetOUT(ALGO_RUN_INDEX, run);
	SetOUT(ALGO_CUT_INDEX, cut);

	BcpWrite(ALGO_LINE_REGISTER, lineCounter);
	// BcpWrite(ALGO_SPIRE_REGISTER, spireCounter);
	BcpWrite(ALGO_DELAY_REGISTER, algoDelay);
	BcpWrite(ALGO_TOTAL_LINES_REGISTER, lineTotal);
}

void AlgoCountSpires()
{
	static char first = 1;
	if(count)
	{
		if(first)
		{
			first = 0;
		}
		else
		{
			spireCounter++;
			spireTotal++;
		}
	}
	if(spireCounter >= spirePreset + plus1)
	{
		cut = 1;
		AlgoFunc = WaitForReset;
	}
}

void WaitForReset()
{
	if(reset)
	{
		lineCounter++;
		lineTotal++;

		if(lineCounter && lineCounter == linePreset)
		{
			readyTotal++;
			BcpWrite(ALGO_TOTAL_READY_REGISTER, readyTotal);
		}
		
		// set delay after which run will turn on
		if(lineCounter < linePreset)
		{
			algoDelayCounter = ALGO_DELAY;
		}
		
		AlgoFunc = AlgoCountSpires;
	}
}

void Reset()
{
	lineCounter = 0;
	spireCounter = 0;
}

void SetSpires(int spires)
{
	spirePreset = spires;
	BcpWrite(ALGO_SPIRE_PRESET_REGISTER, spirePreset);
}

int GetSpireCount()
{
	return spireCounter;
}

int GetSpirePreset()
{
	return spirePreset;
}

void SetLines(int lines)
{
	linePreset = lines;
	BcpWrite(ALGO_LINE_PRESET_REGISTER, linePreset);
}

int GetLineCount()
{
	return lineCounter;
}

int GetLinePreset()
{
	return linePreset;
}

int GetLineTotal()
{
	return lineTotal;
}

int GetReadyTotal()
{
	return readyTotal;
}

int GetDelay()
{
	return algoDelay;
}

void SetDelay(int delay)
{
	algoDelay = delay;
	BcpWrite(ALGO_DELAY_REGISTER, algoDelay);
}

int GetPlus1()
{
	return plus1;
}

int Cut()
{
	return cut;
}

int Run()
{
	return run;
}

void SetRun(int state)
{
	run = state;
}

void ResetStat()
{
	readyTotal = 0;
	BcpWrite(ALGO_TOTAL_READY_REGISTER, readyTotal);
	lineTotal = 0;
}
