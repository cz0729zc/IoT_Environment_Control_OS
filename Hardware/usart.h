//#ifndef __USART_H
//#define __USART_H
//#include "stdio.h"	
//#include "sys.h" 


//#define USART1_REC_LEN  			200  	//定义最大接收字节数 200
//#define USART2_REC_LEN  			200  	//定义最大接收字节数 200
//#define USART3_REC_LEN  			200  	//定义最大接收字节数 200

//#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收
//#define EN_USART2_RX 			1		//使能（1）/禁止（0）串口1接收
//#define EN_USART3_RX 			1		//使能（1）/禁止（0）串口1接收


//extern u8  USART1_RX_BUF[USART1_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
//extern u8  USART2_RX_BUF[USART2_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
//extern u8  USART3_RX_BUF[USART3_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
//extern u8 Usart_Rec_Data[100];//设备串口接收数组
//extern unsigned char Rec_Count;//串口接收计数位，接收的数据必须小于256位！
//extern u8  Usart_Analysis_Flag;//串口接收的分析数据的标志位

//extern unsigned char	EDFA_Count ;//EDFA作为开头，同时第二条EDFA作为结尾

//extern u8 USART1_RX_STA;         		//接收状态标记	
//extern u8 USART2_RX_STA;         		//接收状态标记	
//extern u8 USART3_RX_STA;         		//接收状态标记	


//extern u8 USART1_RX_Com;         		//串口命令标记	
//extern u8 USART3_RX_Com;         		//串口命令标记	



////如果想串口中断接收，请不要注释以下宏定义
//void uart_init(USART_TypeDef* USARTx,u32 bound); 


//void USART_Send_Char(USART_TypeDef* USARTx,u8 UData);//串口发送单个字符数据
//void USART_Send_Str(USART_TypeDef* USARTx,u8 *UData);//串口发送字符串数据	

//#endif


