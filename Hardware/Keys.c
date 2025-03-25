#include "keys.h"
#include "delay.h"

// 按键初始化
void Key_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
	
	// 禁用JTAG，只保留SWD，使PB3、PB4和PA15可以作为普通I/O使用
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE); // 使能GPIOB时钟和AFIO时钟
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); // 禁用JTAG功能，只保留SWD
	
    // 配置按键引脚为浮空输入模式
    GPIO_InitStructure.GPIO_Pin = KEY_UP_PIN | KEY_DOWN_PIN | KEY_ADD_PIN | KEY_SUB_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 上拉输入模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 输入模式下速度无意义
    GPIO_Init(KEY_GPIO_PORT, &GPIO_InitStructure);
}

// 按键扫描（带硬件消抖）
Key_Value Key_Scan(void) {
    static uint8_t key_lock = 0;
    static uint16_t key_press_time = 0;
    static Key_Value last_key = KEY_NONE;

    // 检测到任意按键按下
    if(GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY_UP_PIN) == Bit_RESET ||
       GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY_DOWN_PIN) == Bit_RESET ||
       GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY_ADD_PIN) == Bit_RESET ||
       GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY_SUB_PIN) == Bit_RESET)
    {
        if(!key_lock) {
            delay_ms(10); // 消抖延时
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

        // 长按检测（1秒后加速）
        key_press_time++;
        if(key_press_time > 100) { // 1秒后
            if((key_press_time % 5) == 0) { // 每50ms触发一次
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