#ifndef __DHT11_H
#define __DHT11_H
 
#include <stm32f10x.h>
 
//温湿度引脚在这里改
//这里通过宏定义，以方便用户修改引脚
#define DHT_GPIO_PORT		GPIOB
#define DHT_GPIO_PIN		GPIO_Pin_12
#define DHT_RCC_PORT		RCC_APB2Periph_GPIOB
 
uint8_t DHT_Get_Temp_Humi_Data(uint8_t buffer[]);
 
#endif
