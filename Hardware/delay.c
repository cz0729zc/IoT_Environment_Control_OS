#include "delay.h"

static uint8_t  fac_us = 0;
static uint16_t fac_ms = 0;

void delay_init() {
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); // 72MHz/8=9MHz
    fac_us = 9;  // 9个时钟周期=1us
    fac_ms = 9000; // 9000个时钟周期=1ms
}

void delay_us(uint32_t nus) {
    uint32_t temp;
    SysTick->LOAD = nus * fac_us;
    SysTick->VAL = 0x00;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    do {
        temp = SysTick->CTRL;
    } while((temp & 0x01) && !(temp & (1 << 16)));
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL = 0x00;
}

void delay_ms(uint32_t nms) {
    uint32_t temp;
    SysTick->LOAD = nms * fac_ms;
    SysTick->VAL = 0x00;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    do {
        temp = SysTick->CTRL;
    } while((temp & 0x01) && !(temp & (1 << 16)));
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL = 0x00;
}
