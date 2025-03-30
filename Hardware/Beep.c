#include "beep.h"
#include "delay.h"

// 初始化蜂鸣器
void BEEP_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

	// 禁用JTAG，只保留SWD，使PB3、PB4和PA15可以作为普通I/O使用
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE); // 使能GPIOB时钟和AFIO时钟
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); // 禁用JTAG功能，只保留SWD

    // 配置PA15为推挽输出
    GPIO_InitStructure.GPIO_Pin = BEEP_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BEEP_GPIO, &GPIO_InitStructure);

    // 默认关闭蜂鸣器
    BEEP_Off();
}

// 打开蜂鸣器
void BEEP_On(void) {
    GPIO_SetBits(BEEP_GPIO, BEEP_PIN); // PA15输出高电平
}

// 关闭蜂鸣器
void BEEP_Off(void) {
    GPIO_ResetBits(BEEP_GPIO, BEEP_PIN); // PA15输出低电平
}

// 蜂鸣器报警控制
void BEEP_Alarm(uint8_t flag) {
    if (flag) {
        BEEP_On(); // 打开蜂鸣器
    } else {
        BEEP_Off(); // 关闭蜂鸣器
    }
}