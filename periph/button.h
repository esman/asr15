#ifndef BUTTON_H
#define BUTTON_H

#include "gpio.h"

#define BUTTON_UP_GROUP GPIOA
#define BUTTON_UP_PIN 8

#define BUTTON_DOWN_GROUP GPIOC
#define BUTTON_DOWN_PIN 13

#define BUTTON_LEFT_GROUP GPIOC
#define BUTTON_LEFT_PIN 14

#define BUTTON_RIGHT_GROUP GPIOA
#define BUTTON_RIGHT_PIN 12

#define BUTTON_OK_GROUP GPIOC
#define BUTTON_OK_PIN 15

#define BUTTON_GET(_group, _pin) (!GPIO_GET(_group, _pin))

#define BUTTON_UP    BUTTON_GET(BUTTON_UP_GROUP,    BUTTON_UP_PIN)
#define BUTTON_DOWN  BUTTON_GET(BUTTON_DOWN_GROUP,  BUTTON_DOWN_PIN)
#define BUTTON_LEFT  BUTTON_GET(BUTTON_LEFT_GROUP,  BUTTON_LEFT_PIN)
#define BUTTON_RIGHT BUTTON_GET(BUTTON_RIGHT_GROUP, BUTTON_RIGHT_PIN)
#define BUTTON_OK    BUTTON_GET(BUTTON_OK_GROUP,    BUTTON_OK_PIN)

#endif // BUTTON_H
