#ifndef __KEYS_H
#define __KEYS_H

#include "stm32f10x.h"

// �������Ŷ���
#define KEY_UP_PIN      GPIO_Pin_3
#define KEY_DOWN_PIN    GPIO_Pin_4
#define KEY_ADD_PIN     GPIO_Pin_5
#define KEY_SUB_PIN     GPIO_Pin_6
#define KEY_GPIO_PORT   GPIOB
#define KEY_GPIO_CLK    RCC_APB2Periph_GPIOB

// ����ֵ����
typedef enum {
    KEY_NONE = 0,
    KEY_UP,
    KEY_DOWN,
    KEY_ADD,
    KEY_SUB
} Key_Value;

// ��������
void Key_Init(void);
Key_Value Key_Scan(void);

#endif
