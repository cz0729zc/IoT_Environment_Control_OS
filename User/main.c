#include "stm32f10x.h"
#include "DHT11.h"
#include "LCD1602.h"
#include "stdio.h"
#include "Serial.h"
#include "delay.h"
#include "LM2904.h"
#include "PM25.h"
#include "keys.h"
#include "LY69.h"
#include "Beep.h"
#include "WiFi.h"
#include "mqtt.h"
#include "timer1.h"
#include "timer4.h"
#include "timer2.h"
#include "timer3.h"

/* ϵͳ״̬���� */
typedef enum {
    STATE_THRESHOLD = 0,    // ��ֵ���ý���
    STATE_PM25,              // PM2.5��ʾ����
    STATE_REALTIME           // ʵʱ������ʾ����
} SystemState;

/* ��ֵ���ý����е�ѡ���� */
typedef enum {
    SEL_TEMP = 0,   // ѡ���¶�
    SEL_HUMI,       // ѡ��ʪ��
    SEL_NOISE       // ѡ������
} SelectState;

/* ȫ�ֱ��� */
SystemState g_state = STATE_THRESHOLD;  // ��ʼ״̬Ϊ��ֵ����
SelectState g_select = SEL_TEMP;        // ��ʼѡ���¶�

/* ��ֵ */
uint8_t g_temp_thres = 33;
uint8_t g_humi_thres = 80;
uint8_t g_noise_thres = 60;

uint8_t Second = 0;
uint8_t Minute = 0;
uint8_t Hour = 0;
uint8_t Upload_TimeOK = 0;  // �������ݱ�־λ

/* ��ʾλ�ö��� */
#define TEMP_STR_ROW 1  
#define TEMP_STR_COL 0
#define TEMP_VAL_ROW 1
#define TEMP_VAL_COL 3
#define HUMI_STR_ROW 1
#define HUMI_STR_COL 0
#define HUMI_VAL_ROW 1
#define HUMI_VAL_COL 12
#define NOISE_STR_ROW 2
#define NOISE_VAL_ROW 5
#define PM25_STR_ROW 1
#define PM25_STR_COL 0
#define PM25_VAL_ROW 1
#define PM25_VAL_COL 6
#define COLUMN_OFFSET 9

int main(void)
{
    uint8_t dht_buffer[5];
    float temperature, humidity;
    uint16_t noise_adc;
    char display_str[16];

    // ��ʼ��
    LCD_Init();
    LM2904_Init();
    Serial_Init();
    Usart3_Init(115200);
    //USART_Init(USART3,115200);
    
    Key_Init();
    SoilHumidity_Init();
    BEEP_Init();

    // WIFI��ʼ��
    WiFi_ResetIO_Init();
    MQTT_Buff_Init();
    AliIoT_Parameter_Init();

    // ��ʱ����ʼ��
    Timer1_Init(49,7200);   //10Khz�ļ���Ƶ�ʣ�������49Ϊ5ms
    TIM4_Init(5000,7200);   //TIM4��ʼ������ʱʱ�� 5000 * 7200 * 1000/72000000 = 500ms
    
    // ��ʼ��ʾ
    LCD_PrintString(1, 0, "Loading System...");
    delay_ms(1000);
    LCD_Clear();

    // ��ʼ��WiFiģ��
    LCD_PrintString(1, 0, "AT Test...");
    if (WiFi_SendCmd("AT", 20) == 0) {
        Serial_Printf("\r\n->ģ����Ӧ����:%s", WiFi_RX_BUF);
        LCD_PrintString(2, 0, "AT OK");
    } else {
        Serial_Printf("\r\n->ģ������Ӧ��");
        LCD_PrintString(2, 0, "AT FAIL");
    }
    delay_ms(500);
    LCD_Clear();
    
    LCD_PrintString(1, 0, "WIFI init...");
    Serial_Printf("׼������STAģʽ\r\n");
    if(WiFi_SendCmd("AT+CWMODE=1",50)) {
        Serial_Printf("����STAģʽʧ�ܣ�׼������\r\n");
    } else {
        Serial_Printf("����STAģʽ�ɹ�\r\n");
    }	
    
    LCD_PrintString(1, 0, "WIFI Connect...");
    if (WiFi_JoinAP(30) == 0) {
        LCD_PrintString(2, 0, "WIFI OK");
        Serial_Printf("����·�����ɹ���");
    } else {
        LCD_PrintString(2, 0, "WIFI FALL");
        Serial_Printf("����·����ʧ�ܣ�");
    }
    delay_ms(500);
    LCD_Clear();

    while (1) {
        delay_ms(200);
        TIM3_ENABLE_30S();
		
		
        // Serial_Printf("����outֵ�� MQTT_TxDataOutPtr :%d\r\n",MQTT_TxDataOutPtr);
        // Serial_Printf("����inֵ��  MQTT_TxDataInPtr  :%d\r\n\r\n",MQTT_TxDataInPtr);
        // Serial_Printf("����outֵ�� MQTT_RxDataOutPtr :%d\r\n",MQTT_RxDataOutPtr);
        // Serial_Printf("����inֵ��  MQTT_RxDataInPtr  :%d\r\n",MQTT_RxDataInPtr);
       
        //Serial_Printf("����ֵ��  MQTT_RxDataOutPtr[2]  :%d\r\n\r\n",MQTT_RxDataOutPtr[2]);
        if(Connect_flag == 1) {  
			
//			if (ConnectPack_flag == 1) {
//				MQTT_Subscribe(S_TOPIC_NAME, 0);  // �������⣬QoS=0
//				Serial_Printf("�Ѷ������⣺%s\r\n", S_TOPIC_NAME);
//			}			
			
            /* �����ͻ��������� */
            if(MQTT_TxDataOutPtr != MQTT_TxDataInPtr) {
                if((MQTT_TxDataOutPtr[2]==0x10) || 
                   ((MQTT_TxDataOutPtr[2]==0x82) && (ConnectPack_flag==1)) || 
                   (SubcribePack_flag==1)) {    
                    Serial_Printf("��������:0x%x\r\n", MQTT_TxDataOutPtr[2]);
                    MQTT_TxData(MQTT_TxDataOutPtr);
                    MQTT_TxDataOutPtr += BUFF_UNIT;
                    if(MQTT_TxDataOutPtr == MQTT_TxDataEndPtr)
                        MQTT_TxDataOutPtr = MQTT_TxDataBuf[0];
                } 
            }

            /* ������ջ��������� */
            if(MQTT_RxDataOutPtr != MQTT_RxDataInPtr) {
                Serial_Printf("���յ�����:");
                if(MQTT_RxDataOutPtr[2] == 0x20) {             			
                    switch(MQTT_RxDataOutPtr[5]) {					
                        case 0x00 : 
                            Serial_Printf("CONNECT���ĳɹ�\r\n");
                            ConnectPack_flag = 1;
                            break;                                              
                        case 0x01 : 
                            Serial_Printf("�����Ѿܾ�����֧�ֵ�Э��汾��׼������\r\n");
                            Connect_flag = 0;
                            break;
                        case 0x02 : 
                            Serial_Printf("�����Ѿܾ������ϸ�Ŀͻ��˱�ʶ����׼������\r\n");
                            Connect_flag = 0;
                            break;
                        case 0x03 : 
                            Serial_Printf("�����Ѿܾ�������˲����ã�׼������\r\n");
                            Connect_flag = 0;
                            break;
                        case 0x04 : 
                            Serial_Printf("�����Ѿܾ�����Ч���û��������룬׼������\r\n");
                            Connect_flag = 0;
                            break;
                        case 0x05 : 
                            Serial_Printf("�����Ѿܾ���δ��Ȩ��׼������\r\n");
                            Connect_flag = 0;
                            break;		
                        default   : 
                            Serial_Printf("�����Ѿܾ���δ֪״̬��׼������\r\n");
                            Connect_flag = 0;
                            break;								
                    }				
                }
                else if(MQTT_RxDataOutPtr[2] == 0x90) { 
                    switch(MQTT_RxDataOutPtr[6]) {					
                        case 0x00 :
                        case 0x01 : 
                            Serial_Printf("���ĳɹ�\r\n");
                            SubcribePack_flag = 1;
                            Ping_flag = 0;
                            TIM3_ENABLE_30S();
                            TIM2_Int_Init(5000,7199);
                            break;                                             
                        default   : 
                            Serial_Printf("����ʧ�ܣ�׼������\r\n");
                            Connect_flag = 0;
                            break;								
                    }					
                }
                else if(MQTT_RxDataOutPtr[2] == 0xD0) { 
                    Serial_Printf("PING���Ļظ�\r\n");
                    if(Ping_flag == 1) {
                        Ping_flag = 0;
                    } else if(Ping_flag > 1) {
                        Ping_flag = 0;
                        TIM3_ENABLE_30S();
                    }				
                }	
                else if(MQTT_RxDataOutPtr[2] == 0x30) { 
                    Serial_Printf("�������ȼ�0����\r\n");
                    MQTT_DealPushdata_Qs0(MQTT_RxDataOutPtr);
                    MQTT_RxDataOutPtr += BUFF_UNIT;
                    if(MQTT_RxDataOutPtr == MQTT_RxDataEndPtr)
                        MQTT_RxDataOutPtr = MQTT_RxDataBuf[0];
                }
                MQTT_RxDataOutPtr += BUFF_UNIT;
                if(MQTT_RxDataOutPtr == MQTT_RxDataEndPtr)
                    MQTT_RxDataOutPtr = MQTT_RxDataBuf[0];
            } // �������ջ���������
        } 
        else {
            Serial_Printf("��Ҫ���ӷ�����\r\n");
            WiFi_RxCounter = 0;                        
            memset(WiFi_RX_BUF, 0, WiFi_RXBUFF_SIZE);
            if(WiFi_Connect_IoTServer() == 0) {   			     
                Serial_Printf("����TCP���ӳɹ�\r\n");
                Connect_flag = 1;	                           
                WiFi_RxCounter = 0;                        
                memset(WiFi_RX_BUF, 0, WiFi_RXBUFF_SIZE);
                MQTT_Buff_ReInit();
                Serial_Printf("�ѳɹ��������п��Ʋ���!\r\n");
            }				
        }

        // ��������������ʾ���루��ע�Ͳ��֣�
        // ... [ԭ��ע�͵Ĵ���������ʾ����]
    }
}