#ifndef UTILS_H_
#define UTILS_H_

#include <stdint.h>
#include <stm32f10x.h>

#define UINT8_BIT(_bit)  ((uint8_t)  0x1 << (_bit))
#define UINT16_BIT(_bit) ((uint16_t) 0x1 << (_bit))
#define UINT32_BIT(_bit) ((uint32_t) 0x1 << (_bit))

#define DELAY(__t) do{ uint32_t t = (uint32_t) (__t); while(t--) __NOP(); } while(0)

#endif /* UTILS_H_ */
