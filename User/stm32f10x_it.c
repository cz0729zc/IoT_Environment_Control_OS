/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "mqtt.h"
#include "Serial3.h"
#include "Serial.h"
#include "usart2.h"
#include "usart3.h"
#include "timer3.h"
#include "timer4.h"
#include "main.h"
#include "PM25.h"
#include "gizwits_product.h"
#include "gizwits_protocol.h"
/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/*-------------------------------------------------*/
/*函数名：串口1接收中断函数                        */
/*参  数：无                                       */
/*返回值：无                                       */
 /*-------------------------------------------------*/
 void USART1_IRQHandler(void) {
     if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
 		//Serial_Printf("\r\n 进入串口中断");
         uint8_t data = USART_ReceiveData(USART1);
         PM25_ReceiveHandler(data);  // PM2.5数据处理调用
 		//Serial_SendByte(data);
         Serial_RxData = data;
         Serial_RxFlag = 1;
         USART_ClearITPendingBit(USART1, USART_IT_RXNE);
     }
 }

///*-------------------------------------------------*/
///*函数名：串口2接收中断函数                        */
///*参  数：无                                       */
///*返回值：无                                       */
///*-------------------------------------------------*/
//void USART2_IRQHandler(void)   
//{                      
//	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET){  //如果USART_IT_RXNE标志置位，表示有数据到了，进入if分支
//		if(Connect_flag==0){                                //如果Connect_flag等于0，当前还没有连接服务器，处于指令配置状态
//			if(USART2->DR){                                 //处于指令配置状态时，非零值才保存到缓冲区	
//				Usart2_RxBuff[Usart2_RxCounter]=USART2->DR; //保存到缓冲区	
//				Usart2_RxCounter ++;                        //每接收1个字节的数据，Usart2_RxCounter加1，表示接收的数据总量+1 
//			}		
//		}else{		                                        //反之Connect_flag等于1，连接上服务器了	
//			Usart2_RxBuff[Usart2_RxCounter] = USART2->DR;   //把接收到的数据保存到Usart2_RxBuff中				
//			if(Usart2_RxCounter == 0){    					//如果Usart2_RxCounter等于0，表示是接收的第1个数据，进入if分支				
//				TIM_Cmd(TIM4,ENABLE); 
//			}else{                        					//else分支，表示果Usart2_RxCounter不等于0，不是接收的第一个数据
//				TIM_SetCounter(TIM4,0);  
//			}	
//			Usart2_RxCounter ++;         				    //每接收1个字节的数据，Usart2_RxCounter加1，表示接收的数据总量+1 
//		}
//	}
//} 


/*-------------------------------------------------*/
/*函数名：串口3接收中断函数                        */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void USART3_IRQHandler(void)   
{                      
  uint8_t value = 0;
  if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET){
        USART_ClearITPendingBit(USART3,USART_IT_RXNE);
        value = USART_ReceiveData(USART3);
        gizPutData(&value, 1);
  }
}


extern u8 Second;
extern u8 Minute;
extern u8 Hour;
extern u8 Upload_TimeOK;

// /*-------------------------------------------------*/
// /*函数名：定时器4中断服务函数  USART3           */
// /*参  数：无                                       */
// /*返回值：无                                       */
// /*-------------------------------------------------*/
// void TIM4_IRQHandler(void)
// {
// 	if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET){                //如果TIM_IT_Update置位，表示TIM4溢出中断，进入if	
		
// 		memcpy(&MQTT_RxDataInPtr[2],Usart3_RxBuff,Usart3_RxCounter);  //拷贝数据到接收缓冲区
// 		MQTT_RxDataInPtr[0] = Usart3_RxCounter/256;                   //记录数据长度高字节
// 		MQTT_RxDataInPtr[1] = Usart3_RxCounter%256;                   //记录数据长度低字节
// 		MQTT_RxDataInPtr+=BUFF_UNIT;                                  //指针下移
// 		if(MQTT_RxDataInPtr==MQTT_RxDataEndPtr)                       //如果指针到缓冲区尾部了
// 			MQTT_RxDataInPtr = MQTT_RxDataBuf[0];                     //指针归位到缓冲区开头
// 		Usart3_RxCounter = 0;                                         //串口2接收数据量变量清零
// 		TIM_SetCounter(TIM3, 0);                                      //清零定时器6计数器，重新计时ping包发送时间
// 		TIM_Cmd(TIM4, DISABLE);                        				  //关闭TIM4定时器
// 		TIM_SetCounter(TIM4, 0);                        			  //清零定时器4计数器
// 		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);     			  //清除TIM4溢出中断标志 

//         TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
//         Upload_TimeOK = 1;  // 设置数据上传标志		
// 	}
// }

/*-------------------------------------------------*/
/*函数名：定时器4中断服务函数                      */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void TIM4_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
  {
      TIM_ClearITPendingBit(TIM4, TIM_IT_Update );
      gizTimerMs();
  }
}
///*-------------------------------------------------*/
///*函数名：定时器3中断服务函数                      */
///*参  数：无                                       */
///*返回值：无                                       */
///*-------------------------------------------------*/
//void TIM3_IRQHandler(void)
//{
//	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET){   //如果TIM_IT_Update置位，表示TIM3溢出中断，进入if	
//		switch(Ping_flag){                               //判断Ping_flag的状态
//			case 0:										 //如果Ping_flag等于0，表示正常状态，发送Ping报文  
//					MQTT_PingREQ(); 					 //添加Ping报文到发送缓冲区  
//					break;
//			case 1:										 //如果Ping_flag等于1，说明上一次发送到的ping报文，没有收到服务器回复，所以1没有被清除为0，可能是连接异常，我们要启动快速ping模式
//					TIM3_ENABLE_2S(); 					 //我们将定时器6设置为2s定时,快速发送Ping报文
//					MQTT_PingREQ();  					 //添加Ping报文到发送缓冲区  
//					break;
//			case 2:										 //如果Ping_flag等于2，说明还没有收到服务器回复
//			case 3:				                         //如果Ping_flag等于3，说明还没有收到服务器回复
//			case 4:				                         //如果Ping_flag等于4，说明还没有收到服务器回复	
//					MQTT_PingREQ();  					 //添加Ping报文到发送缓冲区 
//					break;
//			case 5:										 //如果Ping_flag等于5，说明我们发送了多次ping，均无回复，应该是连接有问题，我们重启连接
//					Connect_flag = 0;                    //连接状态置0，表示断开，没连上服务器
//					TIM_Cmd(TIM3,DISABLE);               //关TIM3 				
//					break;			
//		}
//		Ping_flag++;           		             		 //Ping_flag自增1，表示又发送了一次ping，期待服务器的回复
//		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);      //清除TIM3溢出中断标志 	
//	}
//}
/*-------------------------------------------------*/
/*函数名：定时器2中断服务函数                      */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/

extern u8 RT_Flag;
extern unsigned char pid_val_mid;//pid_val_mid脉冲宽度
extern u8 Second;
extern u8 Minute;
extern u8 Hour;
extern u8 Upload_TimeOK;

//void TIM2_IRQHandler(void)
//{	
////	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET){  //如果TIM_IT_Update置位，表示TIM2溢出中断，进入if	
////		Read_Sensor();
////		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);     //清除TIM2溢出中断标志 	
////	}
//  
//  
//  static u8 num = 0;
//  
//	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
//  {  //如果TIM_IT_Update置位，表示TIM2溢出中断，进入if	
//    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);     //清除TIM2溢出中断标志 	
//    

//		Upload_TimeOK = 1;
////      Read_Sensor();//上载数据,0.5秒中断
//		
//			num++;
//      if(num >= 2)
//      {
//        num = 0;
//        Second++;
//				if(Second >= 60)
//				{
//					Second = 0;
//					Minute++;
//					if(Minute >= 60)
//					{
//						Minute = 0;
//						Hour ++;
//					}
//				}
//				
////				Dispaly();
//      }
//			
//			
//	}   
//}




/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
