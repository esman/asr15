#include "io.h"
#include "stm32f10x_conf.h"

#define IN1_GROUP GPIOA
#define IN1_PIN GPIO_Pin_3
#define IN2_GROUP GPIOA
#define IN2_PIN GPIO_Pin_2
#define IN3_GROUP GPIOA
#define IN3_PIN GPIO_Pin_4

#define OUT1_GROUP GPIOA
#define OUT1_PIN GPIO_Pin_6
#define OUT2_GROUP GPIOA
#define OUT2_PIN GPIO_Pin_7

void InitIO()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_InitStructure.GPIO_Pin = OUT1_PIN;
	GPIO_Init(OUT1_GROUP, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = OUT2_PIN;
	GPIO_Init(OUT2_GROUP, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	
	GPIO_InitStructure.GPIO_Pin = IN1_PIN;
	GPIO_Init(IN1_GROUP, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = IN2_PIN;
	GPIO_Init(IN2_GROUP, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = IN3_PIN;
	GPIO_Init(IN3_GROUP, &GPIO_InitStructure);
}

int GetIN(int index)
{
	switch(index)
	{
	case 0: return !GPIO_ReadInputDataBit(IN1_GROUP, IN1_PIN);
	case 1: return !GPIO_ReadInputDataBit(IN2_GROUP, IN2_PIN);
	case 2: return !GPIO_ReadInputDataBit(IN3_GROUP, IN3_PIN);
	default: break;
	}
	return 0;
}

void SetOUT(int index, int state)
{
	if(state)
	{
		switch(index)
		{
		case 0: GPIO_SetBits(OUT1_GROUP,OUT1_PIN); break;
		case 1: GPIO_SetBits(OUT2_GROUP,OUT2_PIN); break;
		default: break;
		}
	}
	else
	{
		switch(index)
		{
		case 0: GPIO_ResetBits(OUT1_GROUP,OUT1_PIN); break;
		case 1: GPIO_ResetBits(OUT2_GROUP,OUT2_PIN); break;
		default: break;
		}
	}
}

void ProcIO()
{

}
