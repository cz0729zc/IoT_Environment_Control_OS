#include "lcd1602.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

// 精确延时函数（72MHz主频）
static void delay_us(uint32_t us) {
    us = us * 9; // 72MHz / 8 = 9MHz
    while(us--) {
        __NOP(); __NOP(); __NOP(); __NOP(); 
        __NOP(); __NOP(); __NOP(); __NOP();
    }
}

static void delay_ms(uint32_t ms) {
    while(ms--) {
        delay_us(1000);
    }
}

// GPIO初始化
static void GPIO_Config(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
    
    // 启用GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // 配置控制线（RS, RW, EN）
    GPIO_InitStruct.GPIO_Pin = LCD_RS_PIN | LCD_RW_PIN | LCD_EN_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // 配置数据线（PA0-PA7）
    GPIO_InitStruct.GPIO_Pin = LCD_DATA_PINS;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
}

// 写命令
void LCD_WriteCommand(uint8_t cmd) {
    GPIO_ResetBits(LCD_RS_GPIO, LCD_RS_PIN); // RS=0
    GPIO_ResetBits(LCD_RW_GPIO, LCD_RW_PIN); // RW=0
    
    // 写入数据总线
    GPIO_Write(LCD_DATA_GPIO, (GPIO_ReadOutputData(LCD_DATA_GPIO) & 0xFF00) | cmd);
    
    // 产生EN脉冲
    GPIO_SetBits(LCD_EN_GPIO, LCD_EN_PIN);
    delay_ms(5);
    GPIO_ResetBits(LCD_EN_GPIO, LCD_EN_PIN);
    delay_ms(4);
}

// 写数据
void LCD_WriteData(uint8_t data) {
    GPIO_SetBits(LCD_RS_GPIO, LCD_RS_PIN); // RS=1
    GPIO_ResetBits(LCD_RW_GPIO, LCD_RW_PIN); // RW=0
    
    GPIO_Write(LCD_DATA_GPIO, (GPIO_ReadOutputData(LCD_DATA_GPIO) & 0xFF00) | data);
    
    GPIO_SetBits(LCD_EN_GPIO, LCD_EN_PIN);
    delay_ms(5);
    GPIO_ResetBits(LCD_EN_GPIO, LCD_EN_PIN);
    delay_ms(4);
}

// 初始化LCD
void LCD_Init(void) {
    GPIO_Config();
    delay_ms(500); // LCD上电复位
    
    LCD_WriteCommand(0x38); // 8位模式，2行显示
    LCD_WriteCommand(0x0C); // 显示开，光标关
    LCD_WriteCommand(0x06); // 光标右移
    LCD_WriteCommand(0x01); // 清屏
    delay_ms(2);
}

// 设置光标位置
void LCD_SetCursor(uint8_t row, uint8_t col) {
	if(row==1)
	{
		LCD_WriteCommand(0x80|(col-1));
	}
	else if(row==2)
	{
		LCD_WriteCommand(0x80|(col-1+0x40));
	}
}

// 显示字符
void LCD_PrintChar(uint8_t x,uint8_t y, char Char) {
	LCD_SetCursor(x,y);
	LCD_WriteData(Char);
}

// 显示数据
void LCD_PrintData(uint8_t x,uint8_t y, uint8_t Char) {
	LCD_SetCursor(x,y);
	LCD_WriteData(Char);
}

// 显示字符串
void LCD_PrintString(uint8_t x,uint8_t y, char *str) {
	unsigned char i;
	LCD_SetCursor(x,y);
	for(i=0;str[i]!='\0';i++)
	{
		LCD_WriteData(str[i]);
	}
}

// 清屏
void LCD_Clear(void) {
    LCD_WriteCommand(0x01);
    delay_ms(2);
}
