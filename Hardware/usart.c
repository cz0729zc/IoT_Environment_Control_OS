//#include "sys.h"
//#include "usart.h"	
//#include "delay.h"
//#include "LCD1602.H"
//////////////////////////////////////////////////////////////////////////////////// 	 
////���ʹ��ucos,����������ͷ�ļ�����.
//#if SYSTEM_SUPPORT_UCOS
//#include "includes.h"					//ucos ʹ��	  
//#endif

//u8 Hex_Table[] = {"0123456789ABCDEF"};//ʮ����������
//	
//#if 1
//#pragma import(__use_no_semihosting)             
////��׼����Ҫ��֧�ֺ���                 
//struct __FILE 
//{ 
//	int handle; 

//}; 

//FILE __stdout;       
////����_sys_exit()�Ա���ʹ�ð�����ģʽ    
//int _sys_exit(int x) 
//{ 
//	x = x; 
//} 
////�ض���fputc���� 
//int fputc(int ch, FILE *f)
//{      
//	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
//    USART1->DR = (u8) ch;      
//	return ch;
//}
//#endif 

///*ʹ��microLib�ķ���*/
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



////����1�жϷ������
////ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
//u8 USART1_RX_BUF[USART1_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//u8 USART2_RX_BUF[USART2_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//u8 USART3_RX_BUF[USART3_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.

//u8 Usart_Rec_Data[100];//�豸���ڽ�������
//u8 Rec_Count = 0;//���ڽ��ռ���λ�����յ����ݱ���С��256λ��
//u8  Usart_Analysis_Flag = 0;//���ڽ��յķ������ݵı�־λ
////����״̬
////bit15��	������ɱ�־
////bit14��	���յ�0x0d
////bit13~0��	���յ�����Ч�ֽ���Ŀ
//u8 USART1_RX_STA  = 0;       //����״̬���	  
//u8 USART2_RX_STA = 0;       //����״̬���	  
//u8 USART3_RX_STA  = 0;       //����״̬���	  

//u8 USART1_RX_Com = 0;         		//����������	��1:���к����2�������������ֵ��3���¶ȹ��ʵ������4��EDFA���¶ȣ�5�����ã���
//u8 USART3_RX_Com = 0;         		//����������	��1:���к����2�������������ֵ��3���¶ȹ��ʵ������4��EDFA���¶ȣ�5�����ã���

//unsigned char	EDFA_Count = 0;//EDFA��Ϊ��ͷ��ͬʱ�ڶ���EDFA��Ϊ��β
//	
//#define Relay_GPIO  GPIOA
//#define Relay1_Pin      GPIO_Pin_12
//#define Relay1(x)  x?   GPIO_WriteBit(Relay_GPIO,Relay1_Pin,Bit_SET):GPIO_WriteBit(Relay_GPIO,Relay1_Pin,Bit_RESET)

//	
//	
//void USART_Send_Char(USART_TypeDef* USARTx,u8 UData)//���ڷ��͵����ַ�����
//{

//	if(USARTx == USART1) //����1��������
//	{
//		USART1->DR = UData;
//		while((USART1->SR&0X40)==0);//�ȴ����ͽ���
//		
//	}
//	else if(USARTx == USART2) //����2��������
//	{
//		USART2->DR = UData;
//		while((USART2->SR&0X40)==0);//�ȴ����ͽ���
//		
//	}
//	else if(USARTx == USART3) //����3��������
//	{
//		USART3->DR = UData;
//		while((USART3->SR&0X40)==0);//�ȴ����ͽ���
//		
//	}

////	delay_us(5);
//	delay_ms(2);
//}

//void USART_Send_Str(USART_TypeDef* USARTx,u8 *UData)//���ڷ����ַ�������
//{
//	while(*UData!='\0')
//	{
//		USART_Send_Char(USARTx,*UData);
//		
//		UData++;
//		
//	}
//}




////��ʼ��IO ����1 
////bound:������
//void uart_init(USART_TypeDef* USARTx,u32 bound)
//{
//    //GPIO�˿�����
//    GPIO_InitTypeDef GPIO_InitStructure;
//	  USART_InitTypeDef USART_InitStructure;
//	  NVIC_InitTypeDef NVIC_InitStructure;
//	 
//	
//	  
//	
//	if(USARTx == USART1)//��ʼ��USart1
//	{
//		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
//		
//		USART_DeInit(USART1);  //��λ����1
//	 //USART1_TX   PA.9
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
//		GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA9
//	 
//		//USART1_RX	  PA.10
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
//		GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA10

//	 //Usart1 NVIC ����

//		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
//		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//��ռ���ȼ�1
//		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�1
//		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
//		NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
//	
//	 //USART ��ʼ������

//	 USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
//	 USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
//	 USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
//	 USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
//	 USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
//	 USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

//		USART_Init(USART1, &USART_InitStructure); //��ʼ������
//		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
//		USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ��� 
//	}
// 	 else if(USARTx == USART2)//��ʼ��USart2
//	{
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART2��GPIOAʱ��
//		
//		USART_DeInit(USART2);  //��λ����2
//	 //USART2_TX   PA.2
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
//		GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA2
//	 
//		//USART2_RX	  PA.3
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
//		GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA3

//	 //Usart2 NVIC ����

//		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
//		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
//		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
//		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
//		NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
//	
//	 //USART ��ʼ������

//	 USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
//	 USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
//	 USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
//	 USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
//	 USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
//	 USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

//		USART_Init(USART2, &USART_InitStructure); //��ʼ������
//		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�
//		USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ��� 
//	} 
//	else if(USARTx == USART3)//��ʼ��USart3
//	{
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3|RCC_APB2Periph_GPIOB, ENABLE);	//ʹ��USART3��GPIOBʱ��
//		
//		USART_DeInit(USART3);  //��λ����3
//	 //USART3_TX   PB.10
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB.10
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
//		GPIO_Init(GPIOB, &GPIO_InitStructure); //��ʼ��PB10
//	 
//		//USART3_RX	  PB.11
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
//		GPIO_Init(GPIOB, &GPIO_InitStructure);  //��ʼ��PB10

//	 //Usart3 NVIC ����

//		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
//		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�2
//		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�2
//		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
//		NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
//	
//	 //USART ��ʼ������

//	 USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
//	 USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
//	 USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
//	 USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
//	 USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
//	 USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

//		USART_Init(USART3, &USART_InitStructure); //��ʼ������
//		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�����ж�
//		USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ��� 
//	} 
//}


//extern u8 Second;
//extern u8 Minute;
//extern u8 Hour; //����ʱ����
//extern u16  Price;//����
//extern u16 Max_Weight;
//extern void Get_Maopi();

//#if EN_USART1_RX   //���ʹ���˽���
//void USART1_IRQHandler(void)                	//����1�жϷ������
//{
//	u8 i= 0;
//	static u8 num = 0;
//	

//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
//	{
//		USART_ClearFlag(USART1,RESET);
//		
//		USART1_RX_BUF[num] = USART_ReceiveData(USART1);//(USART1->DR);	//��ȡ���յ�������SJ00:00\r\n
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
//				if((USART1_RX_BUF[0] == 'S')&&(USART1_RX_BUF[1] == 'J')) //�޸�ʱ��
//				{
//					Hour = (USART1_RX_BUF[2] - 0x30) * 10 + (USART1_RX_BUF[3] - 0x30);
//					Minute = (USART1_RX_BUF[5] - 0x30) * 10 + (USART1_RX_BUF[6] - 0x30);
//				}
//				else if((USART1_RX_BUF[0] == 'J')&&(USART1_RX_BUF[1] == 'G')) //�۸�
//				{
//					Price = (USART1_RX_BUF[2] - 0x30) * 100 + (USART1_RX_BUF[3] - 0x30) * 10 + (USART1_RX_BUF[5] - 0x30);
//				}
//				else if((USART1_RX_BUF[0] == 'Y')&&(USART1_RX_BUF[1] == 'Z')) //��ֵ
//				{
//					Max_Weight = (USART1_RX_BUF[2] - 0x30) * 1000 + (USART1_RX_BUF[3] - 0x30) * 100 + (USART1_RX_BUF[4] - 0x30) * 10 + (USART1_RX_BUF[5] - 0x30);
//				}
//				else if((USART1_RX_BUF[0] == 'Q')&&(USART1_RX_BUF[1] == 'P')) //ȥƤ
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


//#if EN_USART2_RX   //���ʹ���˽���
//void USART2_IRQHandler(void)                	//����1�жϷ������
//{
//	u8 Res,n,i;
//	static u8 num = 0;
//	

//	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
//	{
//	
//		USART2_RX_BUF[num] = USART_ReceiveData(USART2);//(USART2->DR);	//��ȡ���յ�������
//		
//			num++;
//		if(num>=255)
//			num = 0;
//		
//			
//	}
//}
//#endif	

//#if EN_USART3_RX   //���ʹ���˽���
//void USART3_IRQHandler(void)                	//����1�жϷ������
//{
//  u8 i=0;
//	static u8 Res;

//	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0xED 0xFA��β)
//	{
//		
//		USART3_RX_BUF[Res] = USART_ReceiveData(USART3);
//		
//		
//   }
//}
//#endif	









