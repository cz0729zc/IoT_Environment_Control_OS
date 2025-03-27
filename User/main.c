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
    Serial3_Init();
    //PM25_Init();
    Key_Init();
	SoilHumidity_Init();
	BEEP_Init();

    WiFi_ResetIO_Init();
    MQTT_Buff_Init();
    AliIoT_Parameter_Init();

    // ��ʼ��ʾ
    LCD_PrintString(1, 0, "Loading System...");
	
    delay_ms(1000);
    LCD_Clear();
	

    // ��ʼ��WiFiģ��
    LCD_PrintString(1, 0, "AT Test...");
    
    if (WiFi_SendCmd("AT+RST", 20) == 0) {
        Serial_Printf("\r\n-> ģ����Ӧ����: %s", WiFi_RX_BUF);
        LCD_PrintString(2, 0, "AT OK");
    } else {
        Serial_Printf("\r\n-> ģ������Ӧ��");
        LCD_PrintString(2, 0, "AT FAIL");
    }
    delay_ms(500);
    LCD_Clear();
    
//    LCD_PrintString(1, 0, "WIFI init...");
//    Serial_Printf("׼������STAģʽ\r\n");                  //������ʾ����
//    if(WiFi_SendCmd("AT+CWMODE=1",50)){                //����STAģʽ��100ms��ʱ��λ���ܼ�5s��ʱʱ��
//        Serial_Printf("����STAģʽʧ�ܣ�׼������\r\n");    //���ط�0ֵ������if��������ʾ����
//    }else {
//        Serial_Printf("����STAģʽ�ɹ�\r\n");            //������ʾ����
//    }	
//    
//    LCD_PrintString(1, 0, "WIFI Connect...");
//    if (WiFi_JoinAP(30) == 0) {
//        LCD_PrintString(2, 0, "WIFI OK");
//        Serial_Printf("����·�����ɹ���");
//    } else {
//        LCD_PrintString(2, 0, "WIFI FALL");
//        Serial_Printf("����·����ʧ�ܣ�");
//    }

//    delay_ms(500);
//    LCD_Clear();

//    LCD_PrintString(1, 0, "MQTT Connect...");
//    if (WiFi_Connect_IoTServer() == 0) {
//        LCD_PrintString(2, 0, "WIFI OK");
//        Serial_Printf("����MQTT�������ɹ���");
//    } else {
//        LCD_PrintString(2, 0, "WIFI FAIL");
//        Serial_Printf("����MQTT������ʧ�ܣ�");
//    }
    while (1) {
		//Serial3_SendByte(0xAA);
		//delay_ms(10);
		//Serial3_Printf("\r\n usart3 test");
		//Serial3_SendByte(0xCC);
		
		
        // ����1������ATָ�����ģ����Ӧ
        //Serial_Printf("\r\n[1] ����ATָ�����...\r\n");	
			
		
//        // ��������
//        Key_Value key = Key_Scan();

//        switch (key) {
//            case KEY_UP:  // �л�����
//                g_state = (g_state + 1) % 3;
//                LCD_Clear();
//                break;

//            case KEY_DOWN:  // �л�ѡ���������ֵ���ý�����Ч��
//                if (g_state == STATE_THRESHOLD) {
//                    g_select = (g_select + 1) % 3;
//                }
//                break;

//            case KEY_ADD:  // ����ѡ�в�����ֵ
//                if (g_state == STATE_THRESHOLD) {
//                    switch (g_select) {
//                        case SEL_TEMP: g_temp_thres++; break;
//                        case SEL_HUMI: g_humi_thres++; break;
//                        case SEL_NOISE: g_noise_thres++; break;
//                    }
//                }
//                break;

//            case KEY_SUB:  // ����ѡ�в�����ֵ
//                if (g_state == STATE_THRESHOLD) {
//                    switch (g_select) {
//                        case SEL_TEMP: g_temp_thres--; break;
//                        case SEL_HUMI: g_humi_thres--; break;
//                        case SEL_NOISE: g_noise_thres--; break;
//                    }
//                }
//                break;
//        }

//        // ����״̬��ʾ��ͬ����
//        switch (g_state) {
//            case STATE_PM25: {
//                // PM2.5��ʾ����
//                PM25_Data pm_data = PM25_GetCurrentData();
//                if (pm_data.data_valid) {
//                    char lcd_line1[17];
//                    snprintf(lcd_line1, sizeof(lcd_line1), "PM2.5:%3.0fug/m3", pm_data.pm25_value);
//                    if (pm_data.pm25_value != 0) {
//                        LCD_Clear();
//                        LCD_PrintString(1, 0, lcd_line1);
//                    }
//					else{
//                        LCD_Clear();
//                        LCD_PrintString(1, 0, "Put on PM2.5");
//					}
//                    pm_data.data_valid = 0;
//                } else {
//                    LCD_PrintString(PM25_VAL_ROW, PM25_VAL_COL + 1, "err");
//                }
//				
//                // ��ʾ����ʪ��״̬
//                if (SoilHumidity_Read() == Bit_SET) {
//                    LCD_PrintString(2, 0, "Dry"); // ��������
//                } else {
//                    LCD_PrintString(2, 0, "Wet"); // ����ʪ��
//                }
//                break;
//            }

//            case STATE_THRESHOLD: {
//                // ��ֵ���ý���
//                LCD_PrintString(1, 0, "Tem:  C");
//                sprintf(display_str, "%2d", g_temp_thres);
//                LCD_PrintString(1, 5, display_str);

//                LCD_PrintString(1, 8, " Hum:  %");
//                sprintf(display_str, "%2d", g_humi_thres);
//                LCD_PrintString(1, 13, display_str);

//                LCD_PrintString(2, 0, "Noi:  dB");
//                sprintf(display_str, "%2d", g_noise_thres);
//                LCD_PrintString(2, 5, display_str);

//				// ��ʾѡ�м�ͷ
//				if (g_select == SEL_TEMP) {
//					LCD_ClearChar(2, 9);
//					LCD_PrintChar(1, 8, '<');  // �¶�ѡ�У���ͷ�� (1, 8)
//				} else if (g_select == SEL_HUMI) {
//					LCD_ClearChar(1, 8);
//					LCD_PrintChar(1, 16, '<'); // ʪ��ѡ�У���ͷ�� (1, 16)
//				} else if (g_select == SEL_NOISE) {
//					LCD_ClearChar(1, 16);
//					LCD_PrintChar(2, 9, '<');   // ����ѡ�У���ͷ�� (2, 9)
//					
//				}
//                break;
//            }

//            case STATE_REALTIME: {
//                // ʵʱ������ʾ����
//                if (DHT_Get_Temp_Humi_Data(dht_buffer)) {
//                    LCD_PrintString(1, 0, "T:    C");
//                    LCD_PrintString(1, 10, "H:    %");
//                    LCD_PrintString(2, 0, "Noi:    dB");

//                    temperature = dht_buffer[2] + dht_buffer[3] * 0.1;
//                    humidity = dht_buffer[0] + dht_buffer[1] * 0.1;

//                    // �����¶���ֵ
//                    sprintf(display_str, "%4.1f", temperature);
//                    LCD_PrintString(TEMP_VAL_ROW, TEMP_VAL_COL, display_str);

//                    // ����ʪ����ֵ
//                    sprintf(display_str, "%3.1f", humidity);
//                    LCD_PrintString(HUMI_VAL_ROW, HUMI_VAL_COL, display_str);
//                } else {
//                    LCD_PrintString(TEMP_VAL_ROW, TEMP_VAL_COL, "ERR");
//                    LCD_PrintString(HUMI_VAL_ROW, HUMI_VAL_COL, "ERR");
//                }
//				
//				// �¶ȳ�����ֵʱ�򿪷�����
//				if (temperature > g_temp_thres) {
//					BEEP_Alarm(1); // �򿪷�����
//				} else {
//					BEEP_Alarm(0); // �رշ�����
//				}
//			
//                // ��ʾ��������
//                noise_adc = LM2904_ReadValue();
//                sprintf(display_str, "%3d", noise_adc);
//                LCD_PrintString(NOISE_STR_ROW, 5, display_str);
//                break;
//				
//            }
//        }

//        delay_ms(200);  // ˢ������
    }
}