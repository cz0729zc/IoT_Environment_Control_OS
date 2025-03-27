/*-------------------------------------------------*/
/*            ��γ����STM32ϵ�п�����              */
/*-------------------------------------------------*/
/*                                                 */
/*            ����602Wifi���ܵ�Դ�ļ�              */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f10x.h"  //������Ҫ��ͷ�ļ�
#include "wifi.h"	    //������Ҫ��ͷ�ļ�
#include "iic.h"        //������Ҫ��ͷ�ļ�
#include "24c02.h" 		//������Ҫ��ͷ�ļ�
#include "delay.h"	    //������Ҫ��ͷ�ļ�
#include "Serial.h"	    //������Ҫ��ͷ�ļ�
#include "mqtt.h"       //������Ҫ��ͷ�ļ�
#include "stdio.h"
#include "String.h"
char wifi_mode = 0;     //����ģʽ 0��SSID������д�ڳ�����   1��Smartconfig��ʽ��APP����
	
/*-------------------------------------------------*/
/*����������ʼ��WiFi�ĸ�λIO                       */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void WiFi_ResetIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;                      //����һ������IO�˿ڲ����Ľṹ��
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE);   //ʹ��PA�˿�ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;                 //׼������PB7
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;         //����50Mhz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   		  //���������ʽ
	GPIO_Init(GPIOB, &GPIO_InitStructure);            		  //����PA4
	RESET_IO(1);                                              //��λIO���ߵ�ƽ
}
/*-------------------------------------------------*/
/*��������WiFi��������ָ��                         */
/*��  ����cmd��ָ��                                */
/*��  ����timeout����ʱʱ�䣨100ms�ı�����         */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
char WiFi_SendCmd(char *cmd, int timeout)
{
	WiFi_RxCounter=0;                           //WiFi������������������                        
	memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);     //���WiFi���ջ����� 
	Serial_Printf("\r\n cmd:%s\r\n",cmd);
	//Serial3_SendString(cmd);
	
	WiFi_printf("%s\r\n",cmd);                  //����ָ��
	//Serial_Printf("\r\n ָ��ͳɹ�");
	while(timeout--){                           //�ȴ���ʱʱ�䵽0
		delay_ms(100);                          //��ʱ100ms
		if(!strstr(WiFi_RX_BUF,"AT\r\nOK\r\n"))            //������յ�OK��ʾָ��ɹ�
			break;       						//��������whileѭ��
			
		Serial_Printf("\r\n");
		Serial_Printf("%d ",timeout);               //����������ڵĳ�ʱʱ��
	}
	Serial_Printf("\r\n WiFi_RX_BUF:%s",WiFi_RX_BUF);
	Serial_Printf("\r\n");                          //���������Ϣ
	if(timeout<=0)return 1;                     //���timeout<=0��˵����ʱʱ�䵽�ˣ�Ҳû���յ�OK������1
	else return 0;		         				//��֮����ʾ��ȷ��˵���յ�OK��ͨ��break��������while
}
/*-------------------------------------------------*/
/*��������WiFi��IO�ڸ�λ                                 */
/*��  ����timeout����ʱʱ�䣨100ms�ı�����         */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
char WiFi_IO_Reset(int timeout)
{
	RESET_IO(0);                                    //��λIO���͵�ƽ
	delay_ms(500);                                  //��ʱ500ms
	RESET_IO(1);                                    //��λIO���ߵ�ƽ	
	while(timeout--){                               //�ȴ���ʱʱ�䵽0
		delay_ms(100);                              //��ʱ100ms
		if(strstr(WiFi_RX_BUF,"ready")||strstr(WiFi_RX_BUF,"invalid"))             //������յ�ready��ʾ��λ�ɹ�
			break;       						    //��������whileѭ��
		Serial_Printf("%d ",timeout);                   //����������ڵĳ�ʱʱ��
	}
	Serial_Printf("\r\n");                              //���������Ϣ
	if(timeout<=0)return 1;                         //���timeout<=0��˵����ʱʱ�䵽�ˣ�Ҳû���յ�ready������1
	else return 0;		         				    //��֮����ʾ��ȷ��˵���յ�ready��ͨ��break��������while
}
/*-------------------------------------------------*/
/*��������WiFi��ָ�λ                                 */
/*��  ����timeout����ʱʱ�䣨100ms�ı�����         */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
char WiFi_CMD_Reset(int timeout)
{
  u16 i;
  Serial_Printf("׼���˳�͸��\r\n");  //������ʾ��Ϣ
  for(i=0;i<683;i++)
  {
    WiFi_printf("+++");                      //��λָ��,ͻ��͸��
    delay_ms(50);
		
    if(WiFi_SendCmd("AT",50))//�޸Ĺ�,���ﲻ���߼�
    {
      Serial_Printf("��λʧ�ܣ�׼������\r\n");    //������ʾ����                                     //����1
    }
    else 
    {
      Serial_Printf("��λ�ɹ�\r\n");
      break;
    }      
  }
  
  Serial_Printf("�˳�͸��ָ�����·�!\r\n");  //������ʾ��Ϣ
  delay_ms(500); 
  WiFi_printf("\r\n");  //��ʱ150ms
  delay_ms(150);
  WiFi_printf("AT\r\n");  //��ʱ150ms
  delay_ms(150);
  Serial_Printf("��ʼ����!\r\n");  //������ʾ��Ϣ
	WiFi_printf("AT+RST\r\n");                      //��λָ��
	delay_ms(500);                                  //��ʱ500ms
	while(timeout--){                               //�ȴ���ʱʱ�䵽0
		delay_ms(100);                              //��ʱ100ms
		if(strstr(WiFi_RX_BUF,"ready")||strstr(WiFi_RX_BUF,"invalid"))             //������յ�ready��ʾ��λ�ɹ���invalid��ʾҲ�Ǹ�λ�ɹ���
			break;       						    //��������whileѭ��
		Serial_Printf("%d ",timeout);                   //����������ڵĳ�ʱʱ��
	}
	Serial_Printf("\r\n");                              //���������Ϣ
	if(timeout<=0)return 1;                         //���timeout<=0��˵����ʱʱ�䵽�ˣ�Ҳû���յ�ready������1
	else return 0;		         				    //��֮����ʾ��ȷ��˵���յ�ready��ͨ��break��������while
}
/*-------------------------------------------------*/
/*��������WiFi����·����ָ��                       */
/*��  ����timeout����ʱʱ�䣨1s�ı�����            */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
char WiFi_JoinAP(int timeout)
{		
	WiFi_RxCounter=0;                               //WiFi������������������                        
	memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);         //���WiFi���ջ����� 
	WiFi_printf("AT+CWJAP=\"%s\",\"%s\"\r\n",SSID,PASS); //����ָ��	

	Serial_Printf("AT+CWJAP=\"%s\",\"%s\"\r\n",SSID,PASS); //���������Ϣ

	while(timeout--){                               //�ȴ���ʱʱ�䵽0
		delay_ms(1000);                             //��ʱ1s
		if(strstr(WiFi_RX_BUF,"\n\r\nOK")) //������յ�WIFI GOT IP��ʾ�ɹ�����������ԭ�������ʽ��		WIFI GOT IP\n\r\nOK���ϰ汾�����ʽ��WIFI GOT IP\r\n\r\nOK��Ϊ��ͨ�ã��˴���������
			break;       						    //��������whileѭ��
		Serial_Printf("%d ",timeout);                   //����������ڵĳ�ʱʱ��
	}
	Serial_Printf("\r\n");                              //���������Ϣ
	if(timeout<=0)return 1;                         //���timeout<=0��˵����ʱʱ�䵽�ˣ�Ҳû���յ�WIFI GOT IP������1
	return 0;                                       //��ȷ������0
}
/*-------------------------------------------------*/
/*������������TCP��������������͸��ģʽ            */
/*��  ����timeout�� ��ʱʱ�䣨100ms�ı�����        */
/*����ֵ��0����ȷ  ����������                      */
/*-------------------------------------------------*/
char WiFi_Connect_Server(int timeout)
{	
	WiFi_RxCounter=0;                               //WiFi������������������                        
	memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);         //���WiFi���ջ�����   
	WiFi_printf("AT+CIPSTART=\"TCP\",\"%s\",%d\r\n",ServerIP,ServerPort);//�������ӷ�����ָ��
	while(timeout--){                               //�ȴ���ʱ���
		delay_ms(100);                              //��ʱ100ms	
		if(strstr(WiFi_RX_BUF ,"CONNECT"))          //������ܵ�CONNECT��ʾ���ӳɹ�
			break;                                  //����whileѭ��
		if(strstr(WiFi_RX_BUF ,"CLOSED"))           //������ܵ�CLOSED��ʾ������δ����
			return 1;                               //������δ��������1
		if(strstr(WiFi_RX_BUF ,"ALREADY CONNECTED"))//������ܵ�ALREADY CONNECTED�Ѿ���������
			return 2;                               //�Ѿ��������ӷ���2
		Serial_Printf("%d ",timeout);                   //����������ڵĳ�ʱʱ��  
	}
	Serial_Printf("\r\n");                        //���������Ϣ
	if(timeout<=0)return 3;                   //��ʱ���󣬷���3
	else                                      //���ӳɹ���׼������͸��
	{
		Serial_Printf("���ӷ������ɹ���׼������͸��\r\n");  //������ʾ��Ϣ
		WiFi_RxCounter=0;                               //WiFi������������������                        
		memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);         //���WiFi���ջ�����     
		WiFi_printf("AT+CIPSEND\r\n");                  //���ͽ���͸��ָ��
		while(timeout--){                               //�ȴ���ʱ���
			delay_ms(100);                              //��ʱ100ms	
			if(strstr(WiFi_RX_BUF,"\r\nOK\r\n\r\n>"))   //���������ʾ����͸���ɹ�
				break;                          //����whileѭ��
			Serial_Printf("%d ",timeout);           //����������ڵĳ�ʱʱ��  
		}
		if(timeout<=0)return 4;                 //͸����ʱ���󣬷���4	
	}
	return 0;	                                //�ɹ�����0	
}
/*-------------------------------------------------*/
/*��������WiFi_Smartconfig                         */
/*��  ����timeout����ʱʱ�䣨1s�ı�����            */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
char WiFi_Smartconfig(int timeout)
{
	
	WiFi_RxCounter=0;                           //WiFi������������������                        
	memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);     //���WiFi���ջ�����     
	while(timeout--){                           //�ȴ���ʱʱ�䵽0
		delay_ms(1000);                         //��ʱ1s
		if(strstr(WiFi_RX_BUF,"connected"))     //������ڽ��ܵ�connected��ʾ�ɹ�
			break;                              //����whileѭ��  
		Serial_Printf("%d ",timeout);               //����������ڵĳ�ʱʱ��  
	}	
	Serial_Printf("\r\n");                          //���������Ϣ
	if(timeout<=0)return 1;                     //��ʱ���󣬷���1
	return 0;                                   //��ȷ����0
}
/*-------------------------------------------------*/
/*���������ȴ�����·����                           */
/*��  ����timeout����ʱʱ�䣨1s�ı�����            */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
char WiFi_WaitAP(int timeout)
{		
	while(timeout--){                               //�ȴ���ʱʱ�䵽0
		delay_ms(1000);                             //��ʱ1s
		if(strstr(WiFi_RX_BUF,"WIFI GOT IP"))       //������յ�WIFI GOT IP��ʾ�ɹ�
			break;       						    //��������whileѭ��
		Serial_Printf("%d ",timeout);                   //����������ڵĳ�ʱʱ��
	}
	Serial_Printf("\r\n");                              //���������Ϣ
	if(timeout<=0)return 1;                         //���timeout<=0��˵����ʱʱ�䵽�ˣ�Ҳû���յ�WIFI GOT IP������1
	return 0;                                       //��ȷ������0
}
/*-------------------------------------------------*/
/*��������WiFi���ӷ�����                           */
/*��  ������                                       */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
char WiFi_Connect_IoTServer(void)
{	
    Serial_Printf("׼����λģ��\r\n");                     //������ʾ����
    if(WiFi_CMD_Reset(50)){                                //��λ��100ms��ʱ��λ���ܼ�5s��ʱʱ��
        Serial_Printf("��λʧ�ܣ�׼������\r\n");           //���ط�0ֵ������if��������ʾ����
        return 1;                                      //����1
    }else Serial_Printf("��λ�ɹ�\r\n");                   //������ʾ����

    Serial_Printf("׼������STAģʽ\r\n");                  //������ʾ����
    if(WiFi_SendCmd("AT+CWMODE=1",50)){                //����STAģʽ��100ms��ʱ��λ���ܼ�5s��ʱʱ��
        Serial_Printf("����STAģʽʧ�ܣ�׼������\r\n");    //���ط�0ֵ������if��������ʾ����
        return 2;                                      //����2
    }else Serial_Printf("����STAģʽ�ɹ�\r\n");            //������ʾ����
    
    if(wifi_mode==0){                                      //�������ģʽ=0��SSID������д�ڳ����� 
        Serial_Printf("׼��ȡ���Զ�����\r\n");                 //������ʾ����
        if(WiFi_SendCmd("AT+CWAUTOCONN=0",50)){            //ȡ���Զ����ӣ�100ms��ʱ��λ���ܼ�5s��ʱʱ��
            Serial_Printf("ȡ���Զ�����ʧ�ܣ�׼������\r\n");   //���ط�0ֵ������if��������ʾ����
            return 3;                                      //����3
        }else Serial_Printf("ȡ���Զ����ӳɹ�\r\n");           //������ʾ����
                
        Serial_Printf("׼������·����\r\n");                   //������ʾ����	
        if(WiFi_JoinAP(30)){                               //����·����,1s��ʱ��λ���ܼ�30s��ʱʱ��
            Serial_Printf("����·����ʧ�ܣ�׼������\r\n");     //���ط�0ֵ������if��������ʾ����
            return 4;                                      //����4	
        }else Serial_Printf("����·�����ɹ�\r\n");             //������ʾ����			
    }else{                                                 //�������ģʽ=1��Smartconfig��ʽ
        Serial_Printf("׼�������Զ�����\r\n");                 
        if(WiFi_SendCmd("AT+CWAUTOCONN=1",50)){            //�����Զ�����
            Serial_Printf("�����Զ�����ʧ�ܣ�׼������\r\n");
            return 3;                                      
        }else Serial_Printf("�����Զ����ӳɹ�\r\n");           
        
        Serial_Printf("׼������Smartconfig\r\n");              
        if(WiFi_SendCmd("AT+CWSTARTSMART",50)){            //����Smartconfig
            Serial_Printf("����Smartconfigʧ�ܣ�׼������\r\n");
            return 4;                                      
        }else Serial_Printf("����Smartconfig�ɹ�\r\n");       

        Serial_Printf("��ʹ��APP�����������\r\n");            
        if(WiFi_Smartconfig(60)){                          //APP��������
            Serial_Printf("��������ʧ�ܣ�׼������\r\n");       
            return 5;                                      
        }else Serial_Printf("��������ɹ�\r\n");               

        Serial_Printf("׼���ر�Smartconfig\r\n");              
        if(WiFi_SendCmd("AT+CWSTOPSMART",50)){             //�ر�Smartconfig
            Serial_Printf("�ر�Smartconfigʧ�ܣ�׼������\r\n");
            return 6;                                      
        }else Serial_Printf("�ر�Smartconfig�ɹ�\r\n");        
    }
    
    Serial_Printf("׼������͸��\r\n");                     
    if(WiFi_SendCmd("AT+CIPMODE=1",50)){               //����͸��
        Serial_Printf("����͸��ʧ�ܣ�׼������\r\n");       
        return 8;                                      
    }else Serial_Printf("����͸���ɹ�\r\n");               
    
    Serial_Printf("׼���رն�·����\r\n");                 
    if(WiFi_SendCmd("AT+CIPMUX=0",50)){                //�رն�·����
        Serial_Printf("�رն�·����ʧ�ܣ�׼������\r\n");   
        return 9;                                      
    }else Serial_Printf("�رն�·���ӳɹ�\r\n");           
    
    Serial_Printf("׼�����ӷ�����\r\n");                   
    if(WiFi_Connect_Server(100)){                      //���ӷ�����
        Serial_Printf("���ӷ�����ʧ�ܣ�׼������\r\n");     
        return 10;                                     
    }else Serial_Printf("���ӷ������ɹ�\r\n");             
    
    return 0;                                          
}
