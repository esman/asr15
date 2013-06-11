#ifndef ALGORITHM_H_
#define ALGORITHM_H_

void AlgoInit();

void Reset();
void SetSpires(int spires);
int GetSpireCount();
int GetSpirePreset();
void SetLines(int lines);
int GetLineCount();
int GetLinePreset();
void AlgoMain();

int GetDelay();
void SetDelay(int delay);

int GetLineTotal();
int GetReadyTotal();

void ResetStat();

int GetPlus1();
int Cut();
int Run();

void SetRun(int state);

#endif /* ALGORITHM_H_ */
