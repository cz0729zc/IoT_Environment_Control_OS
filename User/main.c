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
#include "gizwits_product.h"
#include "gizwits_protocol.h"
#include "common.h"


/* ϵͳ״̬���� */
typedef enum {
    STATE_PM25 = 0,    // PM2.5��ʾ����
    STATE_THRESHOLD,              // ��ֵ���ý���
    STATE_REALTIME           // ʵʱ������ʾ����
} SystemState;

/* ��ֵ���ý����е�ѡ���� */
typedef enum {
    SEL_TEMP = 0,   // ѡ���¶�
    SEL_HUMI,       // ѡ��ʪ��
    SEL_NOISE       // ѡ������
} SelectState;

/* ȫ�ֱ��� */
SystemState g_state = STATE_REALTIME;  // ��ʼ״̬Ϊ��ֵ����
SelectState g_select = SEL_TEMP;        // ��ʼѡ���¶�

// ��ȫ�ֱ��������
float g_temperature;
float g_humidity;
uint16_t g_noise;
float g_pm25;
bool Soil_status;
bool beep_status;

/* ��ֵ */
uint8_t g_temp_thres = 33;
uint8_t g_humi_thres = 80;
float g_pm25_thres = 60.00;

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



int main(void)
{
    uint8_t dht_buffer[5];
    float temperature, humidity;
    uint16_t noise_adc;
    char display_str[16];

    // ��ʼ��
    LCD_Init();
    LM2904_Init();
	PM25_Init();
    Serial_Init();
    Usart3_Init(9600);             //AT�̼�115200�������ƹ̼�9600
    //USART_Init(USART3,115200);
    
    Key_Init();
    SoilHumidity_Init();
    BEEP_Init();

    // ��ʱ����ʼ��
   // Timer1_Init(49,7200);   //10Khz�ļ���Ƶ�ʣ�������49Ϊ5ms
    //TIM4_Init(5000,7200);   //TIM4��ʼ������ʱʱ�� 5000 * 7200 * 1000/72000000 = 500ms
	TIM4_Init(1000-1, 72-1);  // ��ʼ��1ms��ʱΪ������Э���ṩʱ��
	TIM_Cmd(TIM4, ENABLE);    // ������ʱ��
//	
//    // WIFI��ʼ��
    userInit();
    gizwitsInit();
	
	delay_ms(500);
	gizwitsSetMode(WIFI_SOFTAP_MODE);
	//gizwitsSetMode(WIFI_AIRLINK_MODE);


    // ��ʼ��ʾ
    LCD_PrintString(1, 0, "Loading System...");
	Serial_Printf("Loading System...");
    delay_ms(1000);
    LCD_Clear();

    while (1) {			
		
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
                        case SEL_NOISE: g_pm25_thres++; break;
                    }
                }
                break;

            case KEY_SUB:  // ����ѡ�в�����ֵ
                if (g_state == STATE_THRESHOLD) {
                    switch (g_select) {
                        case SEL_TEMP: g_temp_thres--; break;
                        case SEL_HUMI: g_humi_thres--; break;
                        case SEL_NOISE: g_pm25_thres--; break;
                    }
                }
                break;
        }

        // ����״̬��ʾ��ͬ����
        switch (g_state) {
            case STATE_PM25: {
				//Serial_Printf("����PM2.5��ʾ����");
                // PM2.5��ʾ����
                PM25_Data pm_data = PM25_GetCurrentData();
				
                if (pm_data.data_valid) {
                    char lcd_line1[17];
                    snprintf(lcd_line1, sizeof(lcd_line1), "PM2.5:%4.2fug/m3", pm_data.pm25_value);
					g_pm25 = pm_data.pm25_value;
					Soil_status = SoilHumidity_Read();
                    if (pm_data.pm25_value != 0) {
                        LCD_Clear();
                        LCD_PrintString(1, 0, lcd_line1);
                    }
					else{
                        LCD_Clear();
                        LCD_PrintString(1, 0, "Put on PM2.5");
					}
                    pm_data.data_valid = 0;
					
					// PM2.5������ֵʱ�򿪷�����
					if (g_pm25 > g_pm25_thres) {
						BEEP_Alarm(1); // �򿪷�����
						delay_ms(1000);
						BEEP_Alarm(0);
						break;
					} else {
						BEEP_Alarm(0); // �رշ�����
					}
                } else {
                    LCD_PrintString(PM25_VAL_ROW, 0, "PM2.5:ERR");
                }
				
                // ��ʾ����ʪ��״̬
                if (SoilHumidity_Read() == Bit_SET) {
                    LCD_PrintString(2, 0, "SoilHumi:Dry"); // ��������
                } else {
                    LCD_PrintString(2, 0, "SoilHumi:Wet"); // ����ʪ��
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

                LCD_PrintString(2, 0, "PM2.5:    ug/m3");
                sprintf(display_str, "%4.1f", g_pm25_thres);
                LCD_PrintString(2, 7, display_str);

				// ��ʾѡ�м�ͷ
				if (g_select == SEL_TEMP) {
					LCD_ClearChar(2, 9);
					LCD_PrintChar(1, 8, '<');  // �¶�ѡ�У���ͷ�� (1, 8)
				} else if (g_select == SEL_HUMI) {
					LCD_ClearChar(1, 8);
					LCD_PrintChar(1, 16, '<'); // ʪ��ѡ�У���ͷ�� (1, 16)
				} else if (g_select == SEL_NOISE) {
					LCD_ClearChar(1, 16);
					LCD_PrintChar(2, 16, '<');   // ����ѡ�У���ͷ�� (2, 9)
					
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
			
			
                // ��ʾ��������
                noise_adc = ConvertToDecibel(LM2904_ReadValue());			
				//noise_adc = LM2904_ReadValue();
				g_noise = noise_adc;
                sprintf(display_str, "%3d", noise_adc);
                LCD_PrintString(NOISE_STR_ROW, 7, display_str);

                //��ȡ����ʪ��״̬�ͷ�����״̬
                Soil_status = SoilHumidity_Read();
                beep_status = GPIO_ReadInputDataBit(BEEP_GPIO,BEEP_PIN); // ������״̬


                PM25_Data pm_data = PM25_GetCurrentData();
                if (pm_data.data_valid) {
                    g_pm25 = pm_data.pm25_value;
                }
                break;
				
				
            }
        }
		
        userHandle();
        
        gizwitsHandle((dataPoint_t *)&currentDataPoint);
//		
        delay_ms(200);  // ˢ������
    }
}

