#ifndef __DHT11_H
#define __DHT11_H
 
#include <stm32f10x.h>
 
//��ʪ�������������
//����ͨ���궨�壬�Է����û��޸�����
#define DHT_GPIO_PORT		GPIOB
#define DHT_GPIO_PIN		GPIO_Pin_12
#define DHT_RCC_PORT		RCC_APB2Periph_GPIOB
 
uint8_t DHT_Get_Temp_Humi_Data(uint8_t buffer[]);
 
#endif
