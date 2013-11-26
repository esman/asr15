#ifndef ALGORITHM_H_
#define ALGORITHM_H_

typedef struct
{
  unsigned spires;
  unsigned lines;
  unsigned delay;  // delay before starting new cycle
  unsigned lines_counter;
  unsigned rolls;  // total rolls done
} algo_presets_t;

extern algo_presets_t algo_presets;
extern unsigned algo_spires_counter;
extern unsigned algo_plus1;
extern unsigned algo_run;

void AlgoMain();
void AlgoReset();

#endif /* ALGORITHM_H_ */
