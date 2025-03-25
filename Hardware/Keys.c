#include "keys.h"
#include "delay.h"

// ������ʼ��
void Key_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
	
	// ����JTAG��ֻ����SWD��ʹPB3��PB4��PA15������Ϊ��ͨI/Oʹ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE); // ʹ��GPIOBʱ�Ӻ�AFIOʱ��
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); // ����JTAG���ܣ�ֻ����SWD
	
    // ���ð�������Ϊ��������ģʽ
    GPIO_InitStructure.GPIO_Pin = KEY_UP_PIN | KEY_DOWN_PIN | KEY_ADD_PIN | KEY_SUB_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // ��������ģʽ
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // ����ģʽ���ٶ�������
    GPIO_Init(KEY_GPIO_PORT, &GPIO_InitStructure);
}

// ����ɨ�裨��Ӳ��������
Key_Value Key_Scan(void) {
    static uint8_t key_lock = 0;
    static uint16_t key_press_time = 0;
    static Key_Value last_key = KEY_NONE;

    // ��⵽���ⰴ������
    if(GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY_UP_PIN) == Bit_RESET ||
       GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY_DOWN_PIN) == Bit_RESET ||
       GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY_ADD_PIN) == Bit_RESET ||
       GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY_SUB_PIN) == Bit_RESET)
    {
        if(!key_lock) {
            delay_ms(10); // ������ʱ
            if(GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY_UP_PIN) == Bit_RESET) {
                last_key = KEY_UP;
            } else if(GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY_DOWN_PIN) == Bit_RESET) {
                last_key = KEY_DOWN;
            } else if(GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY_ADD_PIN) == Bit_RESET) {
                last_key = KEY_ADD;
            } else if(GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY_SUB_PIN) == Bit_RESET) {
                last_key = KEY_SUB;
            }
            key_lock = 1;
            key_press_time = 0;
            return last_key;
        }

        // ������⣨1�����٣�
        key_press_time++;
        if(key_press_time > 100) { // 1���
            if((key_press_time % 5) == 0) { // ÿ50ms����һ��
                return last_key;
            }
        }
    } else {
        key_lock = 0;
        key_press_time = 0;
        last_key = KEY_NONE;
    }

    return KEY_NONE;
}