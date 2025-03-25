#include "lcd1602.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

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

// GPIO��ʼ��
static void GPIO_Config(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
    
    // ����GPIOAʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // ���ÿ����ߣ�RS, RW, EN��
    GPIO_InitStruct.GPIO_Pin = LCD_RS_PIN | LCD_RW_PIN | LCD_EN_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // ���������ߣ�PA0-PA7��
    GPIO_InitStruct.GPIO_Pin = LCD_DATA_PINS;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
}

// д����
void LCD_WriteCommand(uint8_t cmd) {
    GPIO_ResetBits(LCD_RS_GPIO, LCD_RS_PIN); // RS=0
    GPIO_ResetBits(LCD_RW_GPIO, LCD_RW_PIN); // RW=0
    
    // д����������
    GPIO_Write(LCD_DATA_GPIO, (GPIO_ReadOutputData(LCD_DATA_GPIO) & 0xFF00) | cmd);
    
    // ����EN����
    GPIO_SetBits(LCD_EN_GPIO, LCD_EN_PIN);
    delay_ms(5);
    GPIO_ResetBits(LCD_EN_GPIO, LCD_EN_PIN);
    delay_ms(4);
}

// д����
void LCD_WriteData(uint8_t data) {
    GPIO_SetBits(LCD_RS_GPIO, LCD_RS_PIN); // RS=1
    GPIO_ResetBits(LCD_RW_GPIO, LCD_RW_PIN); // RW=0
    
    GPIO_Write(LCD_DATA_GPIO, (GPIO_ReadOutputData(LCD_DATA_GPIO) & 0xFF00) | data);
    
    GPIO_SetBits(LCD_EN_GPIO, LCD_EN_PIN);
    delay_ms(5);
    GPIO_ResetBits(LCD_EN_GPIO, LCD_EN_PIN);
    delay_ms(4);
}

// ��ʼ��LCD
void LCD_Init(void) {
    GPIO_Config();
    delay_ms(500); // LCD�ϵ縴λ
    
    LCD_WriteCommand(0x38); // 8λģʽ��2����ʾ
    LCD_WriteCommand(0x0C); // ��ʾ��������
    LCD_WriteCommand(0x06); // �������
    LCD_WriteCommand(0x01); // ����
    delay_ms(2);
}

// ���ù��λ��
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

// ��ʾ�ַ�
void LCD_PrintChar(uint8_t x,uint8_t y, char Char) {
	LCD_SetCursor(x,y);
	LCD_WriteData(Char);
}

// ��ʾ����
void LCD_PrintData(uint8_t x,uint8_t y, uint8_t Char) {
	LCD_SetCursor(x,y);
	LCD_WriteData(Char);
}

// ��ʾ�ַ���
void LCD_PrintString(uint8_t x,uint8_t y, char *str) {
	unsigned char i;
	LCD_SetCursor(x,y);
	for(i=0;str[i]!='\0';i++)
	{
		LCD_WriteData(str[i]);
	}
}

// ����
void LCD_Clear(void) {
    LCD_WriteCommand(0x01);
    delay_ms(2);
}
