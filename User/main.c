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

// ��ȫ�ֱ��������
float g_temperature;
float g_humidity;
uint16_t g_noise;
float g_pm25;

/* ��ֵ */
uint8_t g_temp_thres = 33;
uint8_t g_humi_thres = 80;
uint8_t g_noise_thres = 60;

uint8_t Second = 0;
uint8_t Minute = 0;
uint8_t Hour = 0;
volatile uint8_t Upload_TimeOK = 0;  // �������ݱ�־λ

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

// ��MQTT����ʧ��ʱ��������
#define MAX_RETRY 3
uint8_t retry_count = 0;

void Upload_Sensor_Data() {
	char payload[256];
	
    uint8_t soil_status = SoilHumidity_Read();
    uint8_t beep_status = GPIO_ReadInputDataBit(BEEP_GPIO,BEEP_PIN); // ������״̬
    
    // ����JSON���ݰ�
    snprintf(payload, sizeof(payload),
        "{\"method\":\"thing.event.property.post\","
        "\"id\":\"203302322\","
        "\"params\":{"
            "\"CurrentTemperature\":%.1f,"
            "\"CurrentHumidity\":%.1f,"
            "\"CurrentNosie\":%d,"
            "\"CurrentPM25\":%.0f,"
            "\"SoilHumidity\":%d,"
            "\"OpenBEEP\":%d"
        "},"
        "\"version\":\"1.0.0\"}",
        g_temperature, g_humidity, g_noise, 
        g_pm25, soil_status, beep_status);

    // ����MQTT��Ϣ
    MQTT_PublishQs0(P_TOPIC_NAME, payload, strlen(payload));
}

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
		delay_ms(500);
		LCD_PrintString(2, 0, "Please RST");
		while(!(WiFi_SendCmd("AT", 20) == 0));
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

        if(Connect_flag == 1) {  		
			
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
		
		
        // ��������
        Key_Value key = Key_Scan();

        switch (key) {
            case KEY_UP:  // �л�����
                g_state = (g_state + 1) % 3;
                LCD_Clear();
                break;

            case KEY_DOWN:  // �л�ѡ���������ֵ���ý�����Ч��
                if (g_state == STATE_THRESHOLD) {
                    g_select = (g_select + 1) % 3;
                }
                break;

            case KEY_ADD:  // ����ѡ�в�����ֵ
                if (g_state == STATE_THRESHOLD) {
                    switch (g_select) {
                        case SEL_TEMP: g_temp_thres++; break;
                        case SEL_HUMI: g_humi_thres++; break;
                        case SEL_NOISE: g_noise_thres++; break;
                    }
                }
                break;

            case KEY_SUB:  // ����ѡ�в�����ֵ
                if (g_state == STATE_THRESHOLD) {
                    switch (g_select) {
                        case SEL_TEMP: g_temp_thres--; break;
                        case SEL_HUMI: g_humi_thres--; break;
                        case SEL_NOISE: g_noise_thres--; break;
                    }
                }
                break;
        }

        // ����״̬��ʾ��ͬ����
        switch (g_state) {
            case STATE_PM25: {
                // PM2.5��ʾ����
                PM25_Data pm_data = PM25_GetCurrentData();
                if (pm_data.data_valid) {
                    char lcd_line1[17];
                    snprintf(lcd_line1, sizeof(lcd_line1), "PM2.5:%3.0fug/m3", pm_data.pm25_value);
					g_pm25 = pm_data.pm25_value;
                    if (pm_data.pm25_value != 0) {
                        LCD_Clear();
                        LCD_PrintString(1, 0, lcd_line1);
                    }
					else{
                        LCD_Clear();
                        LCD_PrintString(1, 0, "Put on PM2.5");
					}
                    pm_data.data_valid = 0;
                } else {
                    LCD_PrintString(PM25_VAL_ROW, PM25_VAL_COL + 1, "err");
                }
				
                // ��ʾ����ʪ��״̬
                if (SoilHumidity_Read() == Bit_SET) {
                    LCD_PrintString(2, 0, "Dry"); // ��������
                } else {
                    LCD_PrintString(2, 0, "Wet"); // ����ʪ��
                }
                break;
            }

            case STATE_THRESHOLD: {
                // ��ֵ���ý���
                LCD_PrintString(1, 0, "Tem:  C");
                sprintf(display_str, "%2d", g_temp_thres);
                LCD_PrintString(1, 5, display_str);

                LCD_PrintString(1, 8, " Hum:  %");
                sprintf(display_str, "%2d", g_humi_thres);
                LCD_PrintString(1, 13, display_str);

                LCD_PrintString(2, 0, "Noi:  dB");
                sprintf(display_str, "%2d", g_noise_thres);
                LCD_PrintString(2, 5, display_str);

				// ��ʾѡ�м�ͷ
				if (g_select == SEL_TEMP) {
					LCD_ClearChar(2, 9);
					LCD_PrintChar(1, 8, '<');  // �¶�ѡ�У���ͷ�� (1, 8)
				} else if (g_select == SEL_HUMI) {
					LCD_ClearChar(1, 8);
					LCD_PrintChar(1, 16, '<'); // ʪ��ѡ�У���ͷ�� (1, 16)
				} else if (g_select == SEL_NOISE) {
					LCD_ClearChar(1, 16);
					LCD_PrintChar(2, 9, '<');   // ����ѡ�У���ͷ�� (2, 9)
					
				}
                break;
            }

            case STATE_REALTIME: {
                // ʵʱ������ʾ����
                if (DHT_Get_Temp_Humi_Data(dht_buffer)) {
                    LCD_PrintString(1, 0, "T:    C");
                    LCD_PrintString(1, 10, "H:    %");
                    LCD_PrintString(2, 0, "Noise:   dB");

                    temperature = dht_buffer[2] + dht_buffer[3] * 0.1;
                    humidity = dht_buffer[0] + dht_buffer[1] * 0.1;
					
					g_temperature = temperature; // ����ȫ�ֱ���
					g_humidity = humidity;

                    // �����¶���ֵ
                    sprintf(display_str, "%4.1f", temperature);
                    LCD_PrintString(TEMP_VAL_ROW, TEMP_VAL_COL, display_str);

                    // ����ʪ����ֵ
                    sprintf(display_str, "%3.1f", humidity);
                    LCD_PrintString(HUMI_VAL_ROW, HUMI_VAL_COL, display_str);
                } else {
                    LCD_PrintString(TEMP_VAL_ROW, TEMP_VAL_COL, "ERR");
                    LCD_PrintString(HUMI_VAL_ROW, HUMI_VAL_COL, "ERR");
                }
				
				// �¶ȳ�����ֵʱ�򿪷�����
				if (temperature > g_temp_thres) {
					BEEP_Alarm(1); // �򿪷�����
				} else {
					BEEP_Alarm(0); // �رշ�����
				}
			
                // ��ʾ��������
                noise_adc = ConvertToDecibel(LM2904_ReadValue());			
				//noise_adc = LM2904_ReadValue();
				g_noise = noise_adc;
                sprintf(display_str, "%3d", noise_adc);
                LCD_PrintString(NOISE_STR_ROW, 7, display_str);
                break;
				
				
            }
        }
		
		// ��ʱ�ϴ�����������
		if(Upload_TimeOK == 1) {
			Upload_TimeOK = 0;
			if(Connect_flag == 1) { // ȷ����������
				Upload_Sensor_Data();
			}
		}
		
        delay_ms(200);  // ˢ������
    }
}