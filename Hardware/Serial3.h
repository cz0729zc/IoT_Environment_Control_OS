/* Serial3.h */
#ifndef __SERIAL3_H
#define __SERIAL3_H

#include <stdio.h>

void Serial3_Init(void);
void Serial3_SendByte(uint8_t Byte);
void Serial3_SendArray(uint8_t *Array, uint16_t Length);
void Serial3_SendString(char *String);
void Serial3_SendNumber(uint32_t Number, uint8_t Length);
void Serial3_Printf(char *format, ...);
void Serial3_TxData(unsigned char *data);
uint8_t Serial3_GetRxFlag(void);
uint8_t Serial3_GetRxData(void);

#define SERIAL3_RXBUFF_SIZE 1024  // 定义缓冲区大小

extern uint8_t Serial3_RxBuff[SERIAL3_RXBUFF_SIZE];  // 声明外部缓冲区
extern uint16_t Serial3_RxCounter;                   // 声明接收计数器


#endif
