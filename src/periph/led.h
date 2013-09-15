#ifndef LED_H
#define LED_H

namespace led
{

enum
{
  DIGITS = 4,

  SEG_A = 0x01,
  SEG_B = 0x02,
  SEG_C = 0x04,
  SEG_D = 0x08,
  SEG_E = 0x10,
  SEG_F = 0x20,
  SEG_G = 0x40,
  DOT   = 0x80,

  ALL = SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G | DOT,
};

typedef unsigned state_t;

void SetState(int position, state_t state);

} /* namespace led */

#endif /* LED_H */
