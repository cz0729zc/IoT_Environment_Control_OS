#include "stm32f10x.h"
#include "LY69.h"
#define SOIL_HUMIDITY_GPIO GPIOB
#define SOIL_HUMIDITY_PIN GPIO_Pin_1

// ��ʼ������ʪ��ģ��
void SoilHumidity_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    // ʹ��GPIOBʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // ����PB1Ϊ����ģʽ
    GPIO_InitStructure.GPIO_Pin = SOIL_HUMIDITY_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // ��������ģʽ
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SOIL_HUMIDITY_GPIO, &GPIO_InitStructure);
}

// ��ȡ����ʪ��״̬
uint8_t SoilHumidity_Read(void) {
    return GPIO_ReadInputDataBit(SOIL_HUMIDITY_GPIO, SOIL_HUMIDITY_PIN);
}