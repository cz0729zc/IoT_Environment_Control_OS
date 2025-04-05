#include "DHT11.h"
#include "Serial.h"

/**
  * @����  DHT11_GPIO��ʼ������
  * @����  Mode��ָ����������ģʽ
  * @����ֵ  None
  */

  // ��ȷ��ʱ������72MHz��Ƶ��
static void delay_us(uint32_t us) {
	us = us / 2;
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

void DHT_GPIO_Init(GPIOMode_TypeDef Mode)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(DHT_RCC_PORT,ENABLE);  //�򿪶�Ӧ����ʱ��
	
	GPIO_SetBits(DHT_GPIO_PORT,DHT_GPIO_PIN);
	
	GPIO_InitStructure.GPIO_Mode = Mode;  //ͨ��������ʽ������GPIO��ģʽ
	GPIO_InitStructure.GPIO_Pin = DHT_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(DHT_GPIO_PORT,&GPIO_InitStructure);
}
 

/**
  * @����  DHT11ģ����ʼ�źź���
  * @����  None
  * @����ֵ 1��0����־���źųɹ����
  */
uint8_t DHT_Start(void)
{
	//Serial_SendString("\r\n������ʪ��ģ��start����");
	DHT_GPIO_Init(GPIO_Mode_Out_PP);							//�������ģʽ
	//Serial_SendString("\r\n�����ʪ��ģ��init����");

	GPIO_ResetBits(DHT_GPIO_PORT,DHT_GPIO_PIN);		//�������Ƶ��������20ms�͵�ƽ
	delay_ms(20);
	
	//Serial_SendByte(GPIO_ReadInputDataBit(DHT_GPIO_PORT,DHT_GPIO_PIN));
	
	GPIO_SetBits(DHT_GPIO_PORT,DHT_GPIO_PIN);       //��������ߵ�ƽ�������ͷ�����
	delay_us(30);
	
	//Serial_SendByte(GPIO_ReadInputDataBit(DHT_GPIO_PORT,DHT_GPIO_PIN));
	
	DHT_GPIO_Init(GPIO_Mode_IN_FLOATING);					//����ߵ�ƽ�������ͷ����ߣ�����λ��������ģʽ
	delay_us(30);                                           //��ʱ20~40us���մӻ���Ӧ���ź�
	//Serial_SendString("\r\n");
	//Serial_SendByte(GPIO_ReadInputDataBit(DHT_GPIO_PORT,DHT_GPIO_PIN));
	
	if(!GPIO_ReadInputDataBit(DHT_GPIO_PORT,DHT_GPIO_PIN))   //�жϴӻ��Ƿ����Ӧ��ģʽ
	{
		while(!GPIO_ReadInputDataBit(DHT_GPIO_PORT,DHT_GPIO_PIN));   //whileѭ���ȴ��ӻ��͵�ƽӦ��  ��ŵ͵�ƽʱ�����80us�����忴�����ֲᣩ
		//while(GPIO_ReadInputDataBit(DHT_GPIO_PORT,DHT_GPIO_PIN));    //whileѭ���ȴ��ӻ��ߵ�ƽӦ��   ��Ÿߵ�ƽʱ��87us�����忴�ֲᣩ
		//Serial_SendString("\r\n�����ʪ��ģ��Ӧ��ɹ�");
		return 1;    //����1��ʾӦ��ɹ�
	}
	//Serial_SendString("\r\n�����ʪ��ģ��Ӧ��ʧ��");
	return 0;   //����0��ʾӦ��ʧ��
}
 
/**
  * @����   ����DHT11������8λ������
  * @����  None
  * @����ֵ ���ؽ��յ���8λ����
  �����ֲ᣺����λ�Ĵ��䣺����λΪ��0��ʱ��DHT11�����50~58��s�ĵ͵�ƽ������23~27��s�ĸߵ�ƽ������λ
  Ϊ��1��ʱ��DHT11�������50~58��s�ĵ͵�ƽ������68~74��s�ĸߵ�ƽ��
  �ж�����λΪ1��0�ķ�������whlie�ȴ��ߵ�ƽʱ�������Ȼ����ʱ28us����28us���ٶ�ȡ��ȡ�������ţ�����Ϊ�ߵ�ƽ��������Ϊλ1������Ϊ0
  */
uint8_t DHT_Get_Byte_Data(void) {
    uint8_t temp = 0;  // ��ʼ������Ϊ0
    for (int i = 0; i < 8; i++) {
        temp <<= 1;    // �����ڳ����λ
        
        // 1. �ȴ��͵�ƽ������50us�͵�ƽʱ϶��ʼ��
        while (GPIO_ReadInputDataBit(DHT_GPIO_PORT, DHT_GPIO_PIN) == SET); // �ȴ������ͷ�Ϊ�ߵ�ƽ
        while (GPIO_ReadInputDataBit(DHT_GPIO_PORT, DHT_GPIO_PIN) == RESET); // �ȴ��͵�ƽ�������ߵ�ƽ��ʼ��
        
        // 2. �����ߵ�ƽ����ʱ������0/1
        delay_us(40);  // �ؼ��㣺�ȴ���������0�ĸߵ�ƽʱ�䣨26-28us��
        if (GPIO_ReadInputDataBit(DHT_GPIO_PORT, DHT_GPIO_PIN) == SET) {
            temp |= 0x01;  // �ߵ�ƽ��������30us �� �ж�Ϊ1
            while (GPIO_ReadInputDataBit(DHT_GPIO_PORT, DHT_GPIO_PIN) == SET); // �ȴ��ߵ�ƽ������������һbit�ĵ͵�ƽ��
        } 
        // ����Ϊ0���������ȴ�����һbit�ĵ͵�ƽ�ѿ�ʼ��
    }
    return temp;
}
 
/**
  * @����  ��ȡDHT11���¶�ʪ������
  * @����  buffer[]����Ҫ����һ���洢���ݵ�����
  * @����ֵ ��������У���Ƿ���ȷ 1����ȷ 0��ʧ��
  */
uint8_t DHT_Get_Temp_Humi_Data(uint8_t buffer[])
{
	//Serial_SendString("\r\n������ʪ��ģ��");
	if(DHT_Start())  //if�ж�DHT11�ӻ��Ƿ�Ӧ�����
	{
		//Serial_SendString("\r\n�ɼ���ʪ������");
		buffer[0] = DHT_Get_Byte_Data();  //��ȡ�¶ȵ��������������һ��Ԫ��
		buffer[1] = DHT_Get_Byte_Data();  //��ȡ�¶ȵ�С����������ڶ���Ԫ��
		buffer[2] = DHT_Get_Byte_Data();  //��ȡʪ�ȵ������������������Ԫ��
		buffer[3] = DHT_Get_Byte_Data();  //��ȡ�¶ȵ�С������������ĸ�Ԫ��
		buffer[4] = DHT_Get_Byte_Data();  //��ȡ��У�����ݷ�������ĵ����Ԫ�أ�������������Ƿ��ȡ��ȷ
		//Serial_SendArray(buffer,5);	
	}
	return (buffer[0]+buffer[1]+buffer[2]+buffer[3] == buffer[4]) ? 1 : 0;   //У�������Ƿ�����ȷ
}
