#ifndef __LM2904_H
#define __LM2904_H

#include "stm32f10x.h"


#define LM2904_GPIO_PORT    GPIOB
#define LM2904_GPIO_PIN     GPIO_Pin_0
#define LM2904_ADC_CHANNEL  ADC_Channel_8 

void LM2904_Init(void);
uint16_t LM2904_ReadValue(void);

void LM2904_DisplayUpdate(void); // 新增显示更新函数
#endif
