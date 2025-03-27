/* Serial3.c */
#include "stm32f10x.h"
#include <stdio.h>
#include <stdarg.h>
#include "Serial3.h"
#include "Serial.h"
#include "delay.h"

uint8_t Serial3_RxData;
uint8_t Serial3_RxFlag;

uint16_t Serial3_RxCounter = 0;  // 定义接收计数器
uint8_t Serial3_RxBuff[SERIAL3_RXBUFF_SIZE];  // 定义接收缓冲区

void Serial3_Init(void)
{
       // 配置GPIO
    GPIO_InitTypeDef GPIO_InitStructure; 
	
	// 开启时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	USART_DeInit(USART3);
	

    
    // TX-PB10 复用推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // RX-PB11 上拉输入 ,7
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
   
   // 配置NVIC
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStructure);
	
    // 配置USART3
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART3, &USART_InitStructure);




	
    // 使能接收中断
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    // 使能串口
    USART_Cmd(USART3, ENABLE);
}

void Serial3_SendByte(uint8_t Byte)
{
    USART_SendData(USART3, Byte);
    while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
	delay_us(5);
	delay_ms(2);
}

void Serial3_SendArray(uint8_t *Array, uint16_t Length)
{
    uint16_t i;
    for(i = 0; i < Length; i++)
    {
        Serial3_SendByte(Array[i]);
    }
}

void Serial3_SendString(char *String)
{
    uint8_t i;
    for(i = 0; String[i] != '\0'; i++)
    {
        Serial3_SendByte(String[i]);
    }
}

uint32_t Serial3_Pow(uint32_t X, uint32_t Y)
{
    uint32_t Result = 1;
    while(Y--)
    {
        Result *= X;
    }
    return Result;
}

void Serial3_SendNumber(uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for(i = 0; i < Length; i++)
    {
        Serial3_SendByte(Number / Serial3_Pow(10, Length - i - 1) % 10 + '0');
    }
}

void Serial3_Printf(char *format, ...)
{
    char String[100];
    va_list arg;
    va_start(arg, format);
    vsprintf(String, format, arg);
    va_end(arg);
    Serial3_SendString(String);
}

uint8_t Serial3_GetRxFlag(void)
{
    if(Serial3_RxFlag == 1)
    {
        Serial3_RxFlag = 0;
        return 1;
    }
    return 0;
}

uint8_t Serial3_GetRxData(void)
{
    return Serial3_RxData;
}

void USART3_IRQHandler(void)
{
    if(USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
    {
        uint8_t received_data = USART_ReceiveData(USART3);

        if(Serial3_RxCounter < SERIAL3_RXBUFF_SIZE)
        {
            Serial3_RxBuff[Serial3_RxCounter++] = received_data;
        }
		
        //Serial_Printf("\r\n[USART3] Raw Data: 0x%02X", received_data); // 打印16进制数据
		//Serial_Printf("\r\nSerial3_RxCounter:%d",Serial3_RxCounter);
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
		
		if(Serial3_RxCounter >= SERIAL3_RXBUFF_SIZE)
		{
			Serial_Printf("[USART3] Buffer Overflow!\r\n");
			Serial3_RxCounter = 0; // 重置计数器
		}
    }
}
