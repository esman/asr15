extern "C"
{
#include "algorithm.h"
#include "../periph/button.h"
}

#include "../periph/led.h"
#include "../timer.h"

#define MENU_PORT_LED1 GPIOA
#define MENU_PIN_LED1 15

#define MENU_PORT_LED2 GPIOB
#define MENU_PIN_LED2 3

namespace asr
{
namespace menu
{

typedef enum
{
  STATE_INIT,
  STATE_COUNT_LINES,
  STATE_COUNT_SPIRES,
  STATE_SET_LINES,
  STATE_SET_SPIRES,
  STATE_SET_DELAY,
} state_t;

class Button
{
public:
  void Update(bool state);
  bool Pushed() { return push_ && !hold_; }
  bool Held() { return push_ && hold_; }

private:
  enum
  {
    PUSH_DELAY = 20,   //ms
    HOLD_DELAY = 750, //ms
  };

  tmr::Timer push_timer_;
  tmr::Timer hold_timer_;
  bool push_{false};
  bool hold_{false};
};

class Led
{
public:
  enum
  {
    SYM_B = led::SEG_C | led::SEG_D | led::SEG_E | led::SEG_F | led::SEG_G,
    SYM_C = led::SEG_A | led::SEG_D | led::SEG_E | led::SEG_F,
    SYM_D = led::SEG_B | led::SEG_C | led::SEG_D | led::SEG_E | led::SEG_G,
  };

  void Update();
  void SetNumber(int number);
  int  GetNumber();
  void SetPrefix(led::state_t state);
  void Edit(bool edit);
  void Cycle();
  void Next();

private:
  enum
  {
    DGT_0 = led::SEG_A | led::SEG_B | led::SEG_C | led::SEG_D | led::SEG_E | led::SEG_F,
    DGT_1 = led::SEG_B | led::SEG_C,
    DGT_2 = led::SEG_A | led::SEG_B | led::SEG_G | led::SEG_E | led::SEG_D,
    DGT_3 = led::SEG_A | led::SEG_B | led::SEG_C | led::SEG_D | led::SEG_G,
    DGT_4 = led::SEG_B | led::SEG_C | led::SEG_F | led::SEG_G,
    DGT_5 = led::SEG_A | led::SEG_C | led::SEG_D | led::SEG_F | led::SEG_G,
    DGT_6 = led::SEG_A | led::SEG_C | led::SEG_D | led::SEG_E | led::SEG_F | led::SEG_G,
    DGT_7 = led::SEG_A | led::SEG_B | led::SEG_C,
    DGT_8 = led::SEG_A | led::SEG_B | led::SEG_C | led::SEG_D | led::SEG_E | led::SEG_F | led::SEG_G,
    DGT_9 = led::SEG_A | led::SEG_B | led::SEG_C | led::SEG_D | led::SEG_F | led::SEG_G,

    PREFIX_POS = led::DIGITS - 1,

    BLINK_PERIOD = 600,

    DIGITS = led::DIGITS - 1,
  };

  int  digits_[DIGITS];
  int  pos_{0};
  bool edit_{false};
  tmr::Timer blink_timer_;
  bool blink_{false};
};

static Button bset;
static Button bmode;
static Led indicator;

void Led::Update()
{
  const led::state_t digits[] = { DGT_0, DGT_1, DGT_2, DGT_3, DGT_4, DGT_5, DGT_6, DGT_7, DGT_8, DGT_9 };

  if(edit_)
  {
    if(blink_)
    {
      if(blink_timer_.Expired())
      {
        blink_timer_.Reset(BLINK_PERIOD);
        blink_ = false;
      }
    }
    else
    {
      if(blink_timer_.Expired())
      {
        blink_timer_.Reset(BLINK_PERIOD);
        blink_ = true;
      }
    }
  }

  int pos = DIGITS;

  while(pos--)
  {
    auto state = digits[digits_[pos]];
    if(edit_ && (pos == pos_) && !blink_)
    {
      state = 0;
    }
    led::SetState(pos, state);
  }
}

void Led::SetNumber(int number)
{
  int pos = 0;

  while(pos < DIGITS)
  {
    digits_[pos] = number % 10;
    number /= 10;
    pos++;
  }

  pos_ = 0;
}

int Led::GetNumber()
{
  const int factor[DIGITS] = { 1, 10, 100 };

  auto number = 0;
  int pos = DIGITS;

  while(pos--)
  {
    number += digits_[pos] * factor[pos];
  }
  return number;
}

void Led::SetPrefix(led::state_t state)
{
  led::SetState(PREFIX_POS, state);
}

void Led::Edit(bool edit)
{
  edit_ = edit;

  if(edit_)
  {
    pos_   = 0;
    blink_timer_.Reset(BLINK_PERIOD);
    blink_ = true;
  }
}

void Led::Cycle()
{
  if(++digits_[pos_] > 9)
  {
    digits_[pos_] = 0;
  }
  blink_timer_.Reset(BLINK_PERIOD);
  blink_ = true;
}

void Led::Next()
{
  if(++pos_ == DIGITS)
  {
    pos_ = 0;
  }
  blink_timer_.Reset(BLINK_PERIOD);
  blink_ = false;
}

void Button::Update(bool state)
{
  if(state)
  {
    if(hold_timer_.Expired() && hold_ == false)
    {
      hold_ = true;
      push_ = true;
    }
    else
    {
      push_ = false;
    }
  }
  else
  {
    push_ = false;
    hold_ = false;
    if(push_timer_.Expired() && !hold_timer_.Expired())
    {
      push_ = true;
    }
    push_timer_.Reset(PUSH_DELAY);
    hold_timer_.Reset(HOLD_DELAY);
  }
}

extern "C" void MenuProc(void)
{
  static auto state = STATE_INIT;

  bset.Update(BUTTON_SET);
  bmode.Update(BUTTON_MODE);

  switch(state)
  {
  case STATE_INIT:
    indicator.SetPrefix(Led::SYM_C);
    state = STATE_COUNT_LINES;
    break;
  case STATE_COUNT_LINES:
    indicator.SetNumber(algo_presets.lines_counter);
    if(bset.Held())
    {
      ALGO_RESET;
    }
    else if(bmode.Held())
    {
      indicator.Edit(true);
      indicator.SetNumber(algo_presets.lines);
      state = STATE_SET_LINES;
    }
    else if(bmode.Pushed())
    {
      indicator.SetPrefix(Led::SYM_B);
      state = STATE_COUNT_SPIRES;
    }
    break;
  case STATE_COUNT_SPIRES:
    indicator.SetNumber(algo_spires_counter);
    if(bset.Held())
    {
      ALGO_RESET;
    }
    else if(bmode.Held())
    {
      indicator.SetPrefix(Led::SYM_C);
      indicator.Edit(true);
      indicator.SetNumber(algo_presets.lines);
      state = STATE_SET_LINES;
    }
    else if(bmode.Pushed())
    {
      indicator.SetPrefix(Led::SYM_C);
      state = STATE_COUNT_LINES;
    }
    break;
  case STATE_SET_LINES:
    if(bset.Pushed())
    {
      indicator.Cycle();
    }
    else if(bmode.Pushed())
    {
      indicator.Next();
    }
    else if(bmode.Held())
    {
      algo_presets.lines = indicator.GetNumber();
      indicator.SetPrefix(Led::SYM_B);
      indicator.SetNumber(algo_presets.spires);
      state = STATE_SET_SPIRES;
    }
    break;
  case STATE_SET_SPIRES:
    if(bset.Pushed())
    {
      indicator.Cycle();
    }
    else if(bmode.Pushed())
    {
      indicator.Next();
    }
    else if(bmode.Held())
    {
      algo_presets.spires = indicator.GetNumber();
      indicator.SetPrefix(Led::SYM_D);
      indicator.SetNumber(algo_presets.delay);
      state = STATE_SET_DELAY;
    }
    break;
  case STATE_SET_DELAY:
    if(bset.Pushed())
    {
      indicator.Cycle();
    }
    else if(bmode.Pushed())
    {
      indicator.Next();
    }
    else if(bmode.Held())
    {
      algo_presets.delay = indicator.GetNumber();
      indicator.SetPrefix(Led::SYM_C);
      indicator.Edit(false);
      state = STATE_COUNT_LINES;
    }
    break;
  }

  indicator.Update();

  if(algo_plus1)
  {
    GPIO_SET(MENU_PORT_LED2, MENU_PIN_LED2);
    GPIO_RESET(MENU_PORT_LED1, MENU_PIN_LED1);
  }
  else
  {
    GPIO_SET(MENU_PORT_LED1, MENU_PIN_LED1);
    GPIO_RESET(MENU_PORT_LED2, MENU_PIN_LED2);
  }
}

} /* namespace menu */
} /* namespace asr */
