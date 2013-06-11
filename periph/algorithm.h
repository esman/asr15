#ifndef ALGORITHM_H_
#define ALGORITHM_H_

#include <stdint.h>

void InitAlgo();

void StartAlgo();
void StopAlgo();
void Reset();
void SetSpires(int spires);
int GetSpireCount();
int GetSpirePreset();
void SetLines(int lines);
int GetLineCount();
int GetLinePreset();
int AlgoIsWorking();
void ProcAlgo();

int GetLineTotal();
int GetSpireTotal();
int GetStartTotal();

int GetPlus1();
void AlgoSetAuto(uint8_t value);


#endif /* ALGORITHM_H_ */
