#ifndef __KEYS_H
#define __KEYS_H

#include "stm32f10x.h"

// 按键引脚定义
#define KEY_UP_PIN      GPIO_Pin_3
#define KEY_DOWN_PIN    GPIO_Pin_4
#define KEY_ADD_PIN     GPIO_Pin_5
#define KEY_SUB_PIN     GPIO_Pin_6
#define KEY_GPIO_PORT   GPIOB
#define KEY_GPIO_CLK    RCC_APB2Periph_GPIOB

// 按键值定义
typedef enum {
    KEY_NONE = 0,
    KEY_UP,
    KEY_DOWN,
    KEY_ADD,
    KEY_SUB
} Key_Value;

// 函数声明
void Key_Init(void);
Key_Value Key_Scan(void);

#endif
