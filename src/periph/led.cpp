#include "led.h"
#include "../timer.h"

extern "C"
{
#include "gpio.h"
}

#define LED_PORT_A B
#define LED_PIN_A  11

#define LED_PORT_B B
#define LED_PIN_B  2

#define LED_PORT_C B
#define LED_PIN_C  13

#define LED_PORT_D B
#define LED_PIN_D  14

#define LED_PORT_E B
#define LED_PIN_E  15

#define LED_PORT_F B
#define LED_PIN_F  10

#define LED_PORT_G B
#define LED_PIN_G  12

#define LED_PORT_DOT A
#define LED_PIN_DOT  8

#define LED_PORT_DIG_1 A
#define LED_PIN_DIG_1  12

#define LED_PORT_DIG_2 A
#define LED_PIN_DIG_2  11

#define LED_PORT_DIG_3 A
#define LED_PIN_DIG_3  10

#define LED_PORT_DIG_4 A
#define LED_PIN_DIG_4  9

#define LED_GPIO(_port)  GPIO##_port
#define LED_PORT1(_port) LED_GPIO(_port)
#define LED_PORT(_out)   LED_PORT1(LED_PORT_##_out)
#define LED_PIN(_out)    LED_PIN_##_out

#define LED_SET_OUT(_out, state_) \
  do \
  { \
    if(state_) GPIO_SET(LED_PORT(_out), LED_PIN(_out)); \
    else GPIO_RESET(LED_PORT(_out), LED_PIN(_out)); \
  } while(false)

namespace led
{

state_t digits[DIGITS];
tmr::Timer delay_timer;

static void EnableDigit(int pos, bool enable)
{
  switch(pos)
  {
  case 0:
    LED_SET_OUT(DIG_4, !enable);
    break;
  case 1:
    LED_SET_OUT(DIG_3, !enable);
    break;
  case 2:
    LED_SET_OUT(DIG_2, !enable);
    break;
  case 3:
    LED_SET_OUT(DIG_1, !enable);
    break;
  }
}

static void ShowDigit(int pos, state_t state)
{
  static int prev_pos;

  EnableDigit(prev_pos, false);

  LED_SET_OUT(A, state & SEG_A);
  LED_SET_OUT(B, state & SEG_B);
  LED_SET_OUT(C, state & SEG_C);
  LED_SET_OUT(D, state & SEG_D);
  LED_SET_OUT(E, state & SEG_E);
  LED_SET_OUT(F, state & SEG_F);
  LED_SET_OUT(G, state & SEG_G);
  LED_SET_OUT(DOT, !(state & DOT));

  EnableDigit(pos, true);
  prev_pos = pos;
}

void SetState(int position, state_t state)
{
  digits[position] = state;
}

extern "C" void LedProc()
{
  static int pos;
  static bool initialized;

  if(initialized == false)
  {
    delay_timer.Reset(1000); // 1s
    initialized = true;
  }

  if(++pos == DIGITS)
  {
    pos = 0;
  }

  state_t state = digits[pos];

  if(delay_timer.Running())
  {
    state = ALL;
    if(delay_timer.Expired())
    {
      delay_timer.Stop();
    }
  }

  ShowDigit(pos, state);
}

} /* namespace led */
