#ifndef FRAM_H_
#define FRAM_H_

#include <stdlib.h>
#include <stdint.h>

void FramWrite(uint16_t addr, const void* buff, size_t size);
void FramRead(uint16_t addr, void* buff, size_t size);
void FramInit(void);

#endif /* FRAM_H_ */
