#ifndef GPIO_H_
#define GPIO_H_

#include "../utils.h"

#define GPIO_SET(_port, _pin) do { (_port)->BSRR = UINT16_BIT(_pin); } while(0)
#define GPIO_RESET(_port, _pin) do { (_port)->BRR = UINT16_BIT(_pin); } while(0)
#define GPIO_GET(_port, _pin) (((_port)->IDR & UINT16_BIT(_pin)) != 0)

#endif /* GPIO_H_ */
