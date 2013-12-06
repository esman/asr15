extern "C"
{
#include "algorithm.h"
#include "../periph/io.h"
}

#include "../timer.h"

#define ALGO_COUNT_ON  20
#define ALGO_COUNT_OFF 10
#define ALGO_RESET_ON  200
#define ALGO_RESET_OFF 100

#define ALGO_COUNT_FALLING_EDGE  // detect on falling edge

typedef enum
{
  ALGO_STATE_COUNT,
  ALGO_STATE_RESET
} algo_state_t;

extern "C"
{
algo_presets_t algo_presets;

unsigned algo_spires_counter;
unsigned algo_plus1;

unsigned algo_run;
}

static tmr::Timer delay_timer;
static bool restart;

extern "C" void AlgoMain()
{
  static algo_state_t state = ALGO_STATE_COUNT;
  static unsigned count_filter;
  static unsigned count_state;
  static unsigned reset_filter;
  static unsigned reset_state;
  static unsigned cut;

  unsigned count = 0;

  // Filter count
  if(IO_GET_COUNT)
  {
    if(count_filter < ALGO_COUNT_ON)
    {
      if(++count_filter == ALGO_COUNT_ON && !count_state)
      {
        count_state = 1;
#ifndef ALGO_COUNT_FALLING_EDGE
        count = 1;
#endif /* ALGO_COUNT_FALLING_EDGE */
      }
    }
  }
  else if(count_filter > 0)
  {
    if(--count_filter == ALGO_COUNT_OFF && count_state)
    {
      count_state = 0;
#ifdef ALGO_COUNT_FALLING_EDGE
        count = 1;
#endif /* ALGO_COUNT_FALLING_EDGE */
    }
  }

  // Filter reset
  if(IO_GET_RESET)
  {
    if(reset_filter < ALGO_RESET_ON)
    {
      if(++reset_filter == ALGO_RESET_ON)
      {
        reset_state = 1;
      }
    }
  }
  else if(reset_filter > 0)
  {
    if(--reset_filter == ALGO_RESET_OFF)
    {
      reset_state = 0;
    }
  }

  algo_plus1 = IO_GET_PLUS1 ? 1 : 0;

  if(reset_state)
  {
    algo_spires_counter = 0;
    cut = 0;
  }

  // Run control
  if(count)
  {
    algo_run = 0;
  }
  else if(delay_timer.Expired())
  {
    delay_timer.Stop();
    reset_state = 0;
    algo_run = 1;
  }

  switch (state)
  {
  case ALGO_STATE_COUNT:
    if(algo_presets.spires > 0)
    {
      if(restart)
      {
        restart = false;
        algo_run = 1;
      }
      else if(count)
      {
        if(++algo_spires_counter >= (algo_presets.spires + algo_plus1))
        {
          cut = 1;
          state = ALGO_STATE_RESET;
        }
      }
    }
    break;

  case ALGO_STATE_RESET:
    if(reset_state)
    {
      unsigned lines_counter = ++algo_presets.lines_counter;

      if(lines_counter == algo_presets.lines)
      {
        algo_presets.rolls++;
      }

      // set delay after which run will turn on
      if(lines_counter < algo_presets.lines)
      {
        delay_timer.Reset(algo_presets.delay);
      }

      state = ALGO_STATE_COUNT;
    }
    break;
  }

  restart = false;

  // Set 'run' state
  if(algo_run)
  {
    IO_SET_RUN;
  }
  else
  {
    IO_RESET_RUN;
  }

  // Set 'cut' state
  if(cut)
  {
    IO_SET_CUT;
  }
  else
  {
    IO_RESET_CUT;
  }
  }

  extern "C" void AlgoReset(void)
  {
    if(algo_presets.lines_counter >= algo_presets.lines)
    {
      restart = true;
    }
    algo_presets.lines_counter = 0;
    algo_spires_counter        = 0;
  }
