#include "stm32f10x.h"
#include "LY69.h"
#define SOIL_HUMIDITY_GPIO GPIOB
#define SOIL_HUMIDITY_PIN GPIO_Pin_1

// 初始化土壤湿度模块
void SoilHumidity_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    // 使能GPIOB时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // 配置PB1为输入模式
    GPIO_InitStructure.GPIO_Pin = SOIL_HUMIDITY_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 上拉输入模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SOIL_HUMIDITY_GPIO, &GPIO_InitStructure);
}

// 读取土壤湿度状态
uint8_t SoilHumidity_Read(void) {
    return GPIO_ReadInputDataBit(SOIL_HUMIDITY_GPIO, SOIL_HUMIDITY_PIN);
}