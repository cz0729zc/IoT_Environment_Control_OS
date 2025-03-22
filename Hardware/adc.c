//#include "adc.h"

//volatile uint16_t adc_values[2] = {0}; // 存储两个通道的数据（PM2.5和噪声）

//void ADC_Init(void) {
//    GPIO_InitTypeDef GPIO_InitStructure;
//    ADC_InitTypeDef ADC_InitStructure;
//    DMA_InitTypeDef DMA_InitStructure;

//    // 1. 启用时钟
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);
//    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

//    // 2. 配置PA6和PA1为模拟输入
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_1;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
//    GPIO_Init(GPIOA, &GPIO_InitStructure);

//    // 3. 配置DMA1通道1
//    DMA_DeInit(DMA1_Channel1);
//    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
//    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)adc_values;
//    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
//    DMA_InitStructure.DMA_BufferSize = 2;
//    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
//    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
//    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
//    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
//    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
//    DMA_Cmd(DMA1_Channel1, ENABLE);

//    // 4. 配置ADC1
//    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
//    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
//    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
//    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
//    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
//    ADC_InitStructure.ADC_NbrOfChannel = 2;
//    ADC_Init(ADC1, &ADC_InitStructure);

//    // 5. 配置规则通道（PM2.5和噪声）
//    ADC_RegularChannelConfig(ADC1, PM25_ADC_CHANNEL, 1, ADC_SampleTime_239Cycles5);
//    ADC_RegularChannelConfig(ADC1, NOISE_ADC_CHANNEL, 2, ADC_SampleTime_239Cycles5);

//    // 6. 启用ADC和DMA
//    ADC_DMACmd(ADC1, ENABLE);
//    ADC_Cmd(ADC1, ENABLE);

//    // 7. 校准ADC
//    ADC_ResetCalibration(ADC1);
//    while (ADC_GetResetCalibrationStatus(ADC1));
//    ADC_StartCalibration(ADC1);
//    while (ADC_GetCalibrationStatus(ADC1));

//    // 8. 启动转换
//    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
//}

//uint16_t ADC_Read(uint8_t channel) {
//    if (channel == PM25_ADC_CHANNEL) return adc_values[0];
//    else if (channel == NOISE_ADC_CHANNEL) return adc_values[1];
//    return 0;
//}
