#include "utils.h"
#include "stm32f10x.h"

void Delay(int32_t Val)
{
	while(Val--) __NOP();
}

