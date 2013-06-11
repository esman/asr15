#include "io.h"
#include "stm32f10x_conf.h"

#define IN1_GROUP GPIOB
#define IN1_PIN GPIO_Pin_13
#define IN2_GROUP GPIOB
#define IN2_PIN GPIO_Pin_15
#define IN3_GROUP GPIOA
#define IN3_PIN GPIO_Pin_8
#define IN4_GROUP GPIOA
#define IN4_PIN GPIO_Pin_11
#define IN5_GROUP GPIOA
#define IN5_PIN GPIO_Pin_12
#define IN6_GROUP GPIOB
#define IN6_PIN GPIO_Pin_4
#define IN7_GROUP GPIOB
#define IN7_PIN GPIO_Pin_5
#define IN8_GROUP GPIOB
#define IN8_PIN GPIO_Pin_6

#define OUT1_GROUP GPIOA
#define OUT1_PIN GPIO_Pin_3
#define OUT2_GROUP GPIOA
#define OUT2_PIN GPIO_Pin_7
#define OUT3_GROUP GPIOB
#define OUT3_PIN GPIO_Pin_0
#define OUT4_GROUP GPIOB
#define OUT4_PIN GPIO_Pin_1

void InitIO()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	// GPIO_InitStructure.GPIO_Pin = OUT1_PIN;
	// GPIO_Init(OUT1_GROUP, &GPIO_InitStructure);
	// GPIO_InitStructure.GPIO_Pin = OUT2_PIN;
	// GPIO_Init(OUT2_GROUP, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = OUT3_PIN;
	GPIO_Init(OUT3_GROUP, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = OUT4_PIN;
	GPIO_Init(OUT4_GROUP, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	
	// GPIO_InitStructure.GPIO_Pin = IN1_PIN;
	// GPIO_Init(IN1_GROUP, &GPIO_InitStructure);
	// GPIO_InitStructure.GPIO_Pin = IN2_PIN;
	// GPIO_Init(IN2_GROUP, &GPIO_InitStructure);
	// GPIO_InitStructure.GPIO_Pin = IN3_PIN;
	// GPIO_Init(IN3_GROUP, &GPIO_InitStructure);
	// GPIO_InitStructure.GPIO_Pin = IN4_PIN;
	// GPIO_Init(IN4_GROUP, &GPIO_InitStructure);
	// GPIO_InitStructure.GPIO_Pin = IN5_PIN;
	// GPIO_Init(IN5_GROUP, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = IN6_PIN;
	GPIO_Init(IN6_GROUP, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = IN7_PIN;
	GPIO_Init(IN7_GROUP, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = IN8_PIN;
	GPIO_Init(IN8_GROUP, &GPIO_InitStructure);
}

int GetIN(int index)
{
	switch(index)
	{
	// case 0: return GPIO_ReadInputDataBit(IN1_GROUP, IN1_PIN);
	// case 1: return GPIO_ReadInputDataBit(IN2_GROUP, IN2_PIN);
	// case 2: return GPIO_ReadInputDataBit(IN3_GROUP, IN3_PIN);
	// case 3: return GPIO_ReadInputDataBit(IN4_GROUP, IN4_PIN);
	// case 4: return !GPIO_ReadInputDataBit(IN5_GROUP, IN5_PIN);
	case 5: return !GPIO_ReadInputDataBit(IN6_GROUP, IN6_PIN);
	case 6: return !GPIO_ReadInputDataBit(IN7_GROUP, IN7_PIN);
	case 7: return !GPIO_ReadInputDataBit(IN8_GROUP, IN8_PIN);
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
		// case 0: GPIO_SetBits(OUT1_GROUP,OUT1_PIN); break;
		// case 1: GPIO_SetBits(OUT2_GROUP,OUT2_PIN); break;
		case 2: GPIO_SetBits(OUT3_GROUP,OUT3_PIN); break;
		case 3: GPIO_SetBits(OUT4_GROUP,OUT4_PIN); break;
		default: break;
		}
	}
	else
	{
		switch(index)
		{
		// case 0: GPIO_ResetBits(OUT1_GROUP,OUT1_PIN); break;
		// case 1: GPIO_ResetBits(OUT2_GROUP,OUT2_PIN); break;
		case 2: GPIO_ResetBits(OUT3_GROUP,OUT3_PIN); break;
		case 3: GPIO_ResetBits(OUT4_GROUP,OUT4_PIN); break;
		default: break;
		}
	}
}

void ProcIO()
{

}
