//#include "sys.h"
//#include "usart.h"	
//#include "delay.h"
//#include "LCD1602.H"
//////////////////////////////////////////////////////////////////////////////////// 	 
////如果使用ucos,则包括下面的头文件即可.
//#if SYSTEM_SUPPORT_UCOS
//#include "includes.h"					//ucos 使用	  
//#endif

//u8 Hex_Table[] = {"0123456789ABCDEF"};//十六进制数组
//	
//#if 1
//#pragma import(__use_no_semihosting)             
////标准库需要的支持函数                 
//struct __FILE 
//{ 
//	int handle; 

//}; 

//FILE __stdout;       
////定义_sys_exit()以避免使用半主机模式    
//int _sys_exit(int x) 
//{ 
//	x = x; 
//} 
////重定义fputc函数 
//int fputc(int ch, FILE *f)
//{      
//	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
//    USART1->DR = (u8) ch;      
//	return ch;
//}
//#endif 

///*使用microLib的方法*/
// /* 
//int fputc(int ch, FILE *f)
//{
//	USART_SendData(USART1, (uint8_t) ch);

//	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
//   
//    return ch;
//}
//int GetKey (void)  { 

//    while (!(USART1->SR & USART_FLAG_RXNE));

//    return ((int)(USART1->DR & 0x1FF));
//}
//*/
// 
////u8 ATCIPSEND1[] = {"AT+CIPSEND=0,21\r\n"};



////串口1中断服务程序
////注意,读取USARTx->SR能避免莫名其妙的错误   	
//u8 USART1_RX_BUF[USART1_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//u8 USART2_RX_BUF[USART2_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//u8 USART3_RX_BUF[USART3_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.

//u8 Usart_Rec_Data[100];//设备串口接收数组
//u8 Rec_Count = 0;//串口接收计数位，接收的数据必须小于256位！
//u8  Usart_Analysis_Flag = 0;//串口接收的分析数据的标志位
////接收状态
////bit15，	接收完成标志
////bit14，	接收到0x0d
////bit13~0，	接收到的有效字节数目
//u8 USART1_RX_STA  = 0;       //接收状态标记	  
//u8 USART2_RX_STA = 0;       //接收状态标记	  
//u8 USART3_RX_STA  = 0;       //接收状态标记	  

//u8 USART1_RX_Com = 0;         		//串口命令标记	【1:序列号命令；2：输入输出和阈值；3：温度功率电流命令；4：EDFA的温度；5：设置；】
//u8 USART3_RX_Com = 0;         		//串口命令标记	【1:序列号命令；2：输入输出和阈值；3：温度功率电流命令；4：EDFA的温度；5：设置；】

//unsigned char	EDFA_Count = 0;//EDFA作为开头，同时第二条EDFA作为结尾
//	
//#define Relay_GPIO  GPIOA
//#define Relay1_Pin      GPIO_Pin_12
//#define Relay1(x)  x?   GPIO_WriteBit(Relay_GPIO,Relay1_Pin,Bit_SET):GPIO_WriteBit(Relay_GPIO,Relay1_Pin,Bit_RESET)

//	
//	
//void USART_Send_Char(USART_TypeDef* USARTx,u8 UData)//串口发送单个字符数据
//{

//	if(USARTx == USART1) //串口1发送数据
//	{
//		USART1->DR = UData;
//		while((USART1->SR&0X40)==0);//等待发送结束
//		
//	}
//	else if(USARTx == USART2) //串口2发送数据
//	{
//		USART2->DR = UData;
//		while((USART2->SR&0X40)==0);//等待发送结束
//		
//	}
//	else if(USARTx == USART3) //串口3发送数据
//	{
//		USART3->DR = UData;
//		while((USART3->SR&0X40)==0);//等待发送结束
//		
//	}

////	delay_us(5);
//	delay_ms(2);
//}

//void USART_Send_Str(USART_TypeDef* USARTx,u8 *UData)//串口发送字符串数据
//{
//	while(*UData!='\0')
//	{
//		USART_Send_Char(USARTx,*UData);
//		
//		UData++;
//		
//	}
//}




////初始化IO 串口1 
////bound:波特率
//void uart_init(USART_TypeDef* USARTx,u32 bound)
//{
//    //GPIO端口设置
//    GPIO_InitTypeDef GPIO_InitStructure;
//	  USART_InitTypeDef USART_InitStructure;
//	  NVIC_InitTypeDef NVIC_InitStructure;
//	 
//	
//	  
//	
//	if(USARTx == USART1)//初始化USart1
//	{
//		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
//		
//		USART_DeInit(USART1);  //复位串口1
//	 //USART1_TX   PA.9
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
//		GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA9
//	 
//		//USART1_RX	  PA.10
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
//		GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA10

//	 //Usart1 NVIC 配置

//		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
//		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//抢占优先级1
//		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级1
//		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
//		NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
//	
//	 //USART 初始化设置

//	 USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
//	 USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
//	 USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
//	 USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
//	 USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
//	 USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

//		USART_Init(USART1, &USART_InitStructure); //初始化串口
//		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
//		USART_Cmd(USART1, ENABLE);                    //使能串口 
//	}
// 	 else if(USARTx == USART2)//初始化USart2
//	{
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART2，GPIOA时钟
//		
//		USART_DeInit(USART2);  //复位串口2
//	 //USART2_TX   PA.2
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
//		GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA2
//	 
//		//USART2_RX	  PA.3
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
//		GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA3

//	 //Usart2 NVIC 配置

//		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
//		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
//		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
//		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
//		NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
//	
//	 //USART 初始化设置

//	 USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
//	 USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
//	 USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
//	 USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
//	 USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
//	 USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

//		USART_Init(USART2, &USART_InitStructure); //初始化串口
//		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启中断
//		USART_Cmd(USART2, ENABLE);                    //使能串口 
//	} 
//	else if(USARTx == USART3)//初始化USart3
//	{
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3|RCC_APB2Periph_GPIOB, ENABLE);	//使能USART3，GPIOB时钟
//		
//		USART_DeInit(USART3);  //复位串口3
//	 //USART3_TX   PB.10
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB.10
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
//		GPIO_Init(GPIOB, &GPIO_InitStructure); //初始化PB10
//	 
//		//USART3_RX	  PB.11
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
//		GPIO_Init(GPIOB, &GPIO_InitStructure);  //初始化PB10

//	 //Usart3 NVIC 配置

//		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
//		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级2
//		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级2
//		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
//		NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
//	
//	 //USART 初始化设置

//	 USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
//	 USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
//	 USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
//	 USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
//	 USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
//	 USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

//		USART_Init(USART3, &USART_InitStructure); //初始化串口
//		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启中断
//		USART_Cmd(USART3, ENABLE);                    //使能串口 
//	} 
//}


//extern u8 Second;
//extern u8 Minute;
//extern u8 Hour; //定义时分秒
//extern u16  Price;//单价
//extern u16 Max_Weight;
//extern void Get_Maopi();

//#if EN_USART1_RX   //如果使能了接收
//void USART1_IRQHandler(void)                	//串口1中断服务程序
//{
//	u8 i= 0;
//	static u8 num = 0;
//	

//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
//	{
//		USART_ClearFlag(USART1,RESET);
//		
//		USART1_RX_BUF[num] = USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据SJ00:00\r\n
//		
//		num++;
//		
//		if(num>=30)
//		{
//			num = 0;
//		}
//		
//		
//		
//      if((num>=3) && (USART1_RX_BUF[num-2]==0x0d) && (USART1_RX_BUF[num-1]==0x0a))
//      {
//				if((USART1_RX_BUF[0] == 'S')&&(USART1_RX_BUF[1] == 'J')) //修改时间
//				{
//					Hour = (USART1_RX_BUF[2] - 0x30) * 10 + (USART1_RX_BUF[3] - 0x30);
//					Minute = (USART1_RX_BUF[5] - 0x30) * 10 + (USART1_RX_BUF[6] - 0x30);
//				}
//				else if((USART1_RX_BUF[0] == 'J')&&(USART1_RX_BUF[1] == 'G')) //价格
//				{
//					Price = (USART1_RX_BUF[2] - 0x30) * 100 + (USART1_RX_BUF[3] - 0x30) * 10 + (USART1_RX_BUF[5] - 0x30);
//				}
//				else if((USART1_RX_BUF[0] == 'Y')&&(USART1_RX_BUF[1] == 'Z')) //阈值
//				{
//					Max_Weight = (USART1_RX_BUF[2] - 0x30) * 1000 + (USART1_RX_BUF[3] - 0x30) * 100 + (USART1_RX_BUF[4] - 0x30) * 10 + (USART1_RX_BUF[5] - 0x30);
//				}
//				else if((USART1_RX_BUF[0] == 'Q')&&(USART1_RX_BUF[1] == 'P')) //去皮
//				{
//					Get_Maopi();
//				}
//        num = 0;
//      }
//		

//			
//	}
//}
//#endif	


//#if EN_USART2_RX   //如果使能了接收
//void USART2_IRQHandler(void)                	//串口1中断服务程序
//{
//	u8 Res,n,i;
//	static u8 num = 0;
//	

//	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
//	{
//	
//		USART2_RX_BUF[num] = USART_ReceiveData(USART2);//(USART2->DR);	//读取接收到的数据
//		
//			num++;
//		if(num>=255)
//			num = 0;
//		
//			
//	}
//}
//#endif	

//#if EN_USART3_RX   //如果使能了接收
//void USART3_IRQHandler(void)                	//串口1中断服务程序
//{
//  u8 i=0;
//	static u8 Res;

//	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0xED 0xFA结尾)
//	{
//		
//		USART3_RX_BUF[Res] = USART_ReceiveData(USART3);
//		
//		
//   }
//}
//#endif	









