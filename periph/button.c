#include "button.h"
#include "stm32f10x_conf.h"

#define BUTTON_UP_GROUP GPIOB
#define BUTTON_UP_PIN GPIO_Pin_12

#define BUTTON_DOWN_GROUP GPIOC
#define BUTTON_DOWN_PIN GPIO_Pin_13

#define BUTTON_LEFT_GROUP GPIOC
#define BUTTON_LEFT_PIN GPIO_Pin_14

#define BUTTON_RIGHT_GROUP GPIOA
#define BUTTON_RIGHT_PIN GPIO_Pin_1

#define BUTTON_OK_GROUP GPIOC
#define BUTTON_OK_PIN GPIO_Pin_15

void ButtonInit()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_InitStructure.GPIO_Pin = BUTTON_UP_PIN;
  GPIO_Init(BUTTON_UP_GROUP, &GPIO_InitStructure);  

  GPIO_InitStructure.GPIO_Pin = BUTTON_DOWN_PIN;
  GPIO_Init(BUTTON_DOWN_GROUP, &GPIO_InitStructure);  

  GPIO_InitStructure.GPIO_Pin = BUTTON_LEFT_PIN;
  GPIO_Init(BUTTON_LEFT_GROUP, &GPIO_InitStructure);  

  GPIO_InitStructure.GPIO_Pin = BUTTON_RIGHT_PIN;
  GPIO_Init(BUTTON_RIGHT_GROUP, &GPIO_InitStructure);  

  GPIO_InitStructure.GPIO_Pin = BUTTON_OK_PIN;
  GPIO_Init(BUTTON_OK_GROUP, &GPIO_InitStructure);  
}

int ButtonUp()
{
	return !GPIO_ReadInputDataBit(BUTTON_UP_GROUP, BUTTON_UP_PIN);
}

int ButtonDown()
{
	return !GPIO_ReadInputDataBit(BUTTON_DOWN_GROUP, BUTTON_DOWN_PIN);
}

int ButtonLeft()
{
	return !GPIO_ReadInputDataBit(BUTTON_LEFT_GROUP, BUTTON_LEFT_PIN);
}

int ButtonRight()
{
	return !GPIO_ReadInputDataBit(BUTTON_RIGHT_GROUP, BUTTON_RIGHT_PIN);
}

int ButtonOk()
{
	return !GPIO_ReadInputDataBit(BUTTON_OK_GROUP, BUTTON_OK_PIN);
}
