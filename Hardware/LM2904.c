//#include "LM2904.h"
//#include "adc.h"

//#define FILTER_SIZE 5  // ����ƽ���˲�����

//static float voltage_buffer[FILTER_SIZE];
//static uint8_t buffer_index = 0;

//void LM2904_Init(void) {
//    ADC_Init();  // ��ʼ��ADC
//}

//float LM2904_GetVoltage(void) {
//    // ��ȡԭʼADCֵ��ת��Ϊ��ѹ
//    uint16_t raw = ADC_Read(NOISE_ADC_CHANNEL);
//    return (raw / 4096.0f) * 3.3f;
//}

//float LM2904_ConvertToDB(float voltage) {
//    // ʾ����ʽ������ݴ������ֲ�У׼
//    return (voltage / 3.3f) * 94.0f;  // ����3.3V��Ӧ94dB
//}

//float LM2904_GetFilteredDB(void) {
//    // ����ƽ���˲�
//    voltage_buffer[buffer_index] = LM2904_ConvertToDB(LM2904_GetVoltage());
//    buffer_index = (buffer_index + 1) % FILTER_SIZE;
//    
//    float sum = 0;
//    for (uint8_t i = 0; i < FILTER_SIZE; i++) sum += voltage_buffer[i];
//    return sum / FILTER_SIZE;
//}