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


/* 系统状态定义 */
typedef enum {
    STATE_PM25 = 0,    // PM2.5显示界面
    STATE_THRESHOLD,              // 阈值设置界面
    STATE_REALTIME           // 实时数据显示界面
} SystemState;

/* 阈值设置界面中的选中项 */
typedef enum {
    SEL_TEMP = 0,   // 选中温度
    SEL_HUMI,       // 选中湿度
    SEL_NOISE       // 选中噪声
} SelectState;

/* 全局变量 */
SystemState g_state = STATE_REALTIME;  // 初始状态为阈值设置
SelectState g_select = SEL_TEMP;        // 初始选中温度

// 在全局变量区添加
float g_temperature;
float g_humidity;
uint16_t g_noise;
float g_pm25;
bool Soil_status;
bool beep_status;

/* 阈值 */
uint8_t g_temp_thres = 33;
uint8_t g_humi_thres = 80;
float g_pm25_thres = 60.00;

uint8_t Second = 0;
uint8_t Minute = 0;
uint8_t Hour = 0;
volatile uint8_t Upload_TimeOK = 0;  // 上载数据标志位

/* 显示位置定义 */
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

    // 初始化
    LCD_Init();
    LM2904_Init();
	PM25_Init();
    Serial_Init();
    Usart3_Init(9600);             //AT固件115200，机智云固件9600
    //USART_Init(USART3,115200);
    
    Key_Init();
    SoilHumidity_Init();
    BEEP_Init();

    // 定时器初始化
   // Timer1_Init(49,7200);   //10Khz的计数频率，计数到49为5ms
    //TIM4_Init(5000,7200);   //TIM4初始化，定时时间 5000 * 7200 * 1000/72000000 = 500ms
	TIM4_Init(1000-1, 72-1);  // 初始化1ms定时为机智云协议提供时基
	TIM_Cmd(TIM4, ENABLE);    // 启动定时器
//	
//    // WIFI初始化
    userInit();
    gizwitsInit();
	
	delay_ms(500);
	gizwitsSetMode(WIFI_SOFTAP_MODE);
	//gizwitsSetMode(WIFI_AIRLINK_MODE);


    // 初始显示
    LCD_PrintString(1, 0, "Loading System...");
	Serial_Printf("Loading System...");
    delay_ms(1000);
    LCD_Clear();

    while (1) {			
		
        // 按键处理
        Key_Value key = Key_Scan();

        switch (key) {
            case KEY_UP:  // 切换界面
                g_state = (g_state + 1) % 3;
                LCD_Clear();
                break;

            case KEY_DOWN:  // 切换选中项（仅在阈值设置界面有效）
                if (g_state == STATE_THRESHOLD) {
                    g_select = (g_select + 1) % 3;
                }
                break;

            case KEY_ADD:  // 增加选中参数的值
                if (g_state == STATE_THRESHOLD) {
                    switch (g_select) {
                        case SEL_TEMP: g_temp_thres++; break;
                        case SEL_HUMI: g_humi_thres++; break;
                        case SEL_NOISE: g_pm25_thres++; break;
                    }
                }
                break;

            case KEY_SUB:  // 减少选中参数的值
                if (g_state == STATE_THRESHOLD) {
                    switch (g_select) {
                        case SEL_TEMP: g_temp_thres--; break;
                        case SEL_HUMI: g_humi_thres--; break;
                        case SEL_NOISE: g_pm25_thres--; break;
                    }
                }
                break;
        }

        // 根据状态显示不同内容
        switch (g_state) {
            case STATE_PM25: {
				//Serial_Printf("进入PM2.5显示界面");
                // PM2.5显示界面
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
					
					// PM2.5超过阈值时打开蜂鸣器
					if (g_pm25 > g_pm25_thres) {
						BEEP_Alarm(1); // 打开蜂鸣器
						delay_ms(1000);
						BEEP_Alarm(0);
						break;
					} else {
						BEEP_Alarm(0); // 关闭蜂鸣器
					}
                } else {
                    LCD_PrintString(PM25_VAL_ROW, 0, "PM2.5:ERR");
                }
				
                // 显示土壤湿度状态
                if (SoilHumidity_Read() == Bit_SET) {
                    LCD_PrintString(2, 0, "SoilHumi:Dry"); // 土壤干燥
                } else {
                    LCD_PrintString(2, 0, "SoilHumi:Wet"); // 土壤湿润
                }
                break;
            }

            case STATE_THRESHOLD: {
                // 阈值设置界面
                LCD_PrintString(1, 0, "Tem:  C");
                sprintf(display_str, "%2d", g_temp_thres);
                LCD_PrintString(1, 5, display_str);

                LCD_PrintString(1, 8, " Hum:  %");
                sprintf(display_str, "%2d", g_humi_thres);
                LCD_PrintString(1, 13, display_str);

                LCD_PrintString(2, 0, "PM2.5:    ug/m3");
                sprintf(display_str, "%4.1f", g_pm25_thres);
                LCD_PrintString(2, 7, display_str);

				// 显示选中箭头
				if (g_select == SEL_TEMP) {
					LCD_ClearChar(2, 9);
					LCD_PrintChar(1, 8, '<');  // 温度选中，箭头在 (1, 8)
				} else if (g_select == SEL_HUMI) {
					LCD_ClearChar(1, 8);
					LCD_PrintChar(1, 16, '<'); // 湿度选中，箭头在 (1, 16)
				} else if (g_select == SEL_NOISE) {
					LCD_ClearChar(1, 16);
					LCD_PrintChar(2, 16, '<');   // 噪声选中，箭头在 (2, 9)
					
				}
                break;
            }

            case STATE_REALTIME: {
                // 实时数据显示界面
				
                if (DHT_Get_Temp_Humi_Data(dht_buffer)) {
                    LCD_PrintString(1, 0, "T:    C");
                    LCD_PrintString(1, 10, "H:    %");
                    LCD_PrintString(2, 0, "Noise:   dB");

                    temperature = dht_buffer[2] + dht_buffer[3] * 0.1;
                    humidity = dht_buffer[0] + dht_buffer[1] * 0.1;
					
					g_temperature = temperature; // 更新全局变量
					g_humidity = humidity;

                    // 更新温度数值
                    sprintf(display_str, "%4.1f", temperature);
                    LCD_PrintString(TEMP_VAL_ROW, TEMP_VAL_COL, display_str);

                    // 更新湿度数值
                    sprintf(display_str, "%3.1f", humidity);
                    LCD_PrintString(HUMI_VAL_ROW, HUMI_VAL_COL, display_str);
                } else {
                    LCD_PrintString(TEMP_VAL_ROW, TEMP_VAL_COL, "ERR");
                    LCD_PrintString(HUMI_VAL_ROW, HUMI_VAL_COL, "ERR");
                }
			
			
                // 显示噪声数据
                noise_adc = ConvertToDecibel(LM2904_ReadValue());			
				//noise_adc = LM2904_ReadValue();
				g_noise = noise_adc;
                sprintf(display_str, "%3d", noise_adc);
                LCD_PrintString(NOISE_STR_ROW, 7, display_str);

                //获取土壤湿度状态和蜂鸣器状态
                Soil_status = SoilHumidity_Read();
                beep_status = GPIO_ReadInputDataBit(BEEP_GPIO,BEEP_PIN); // 蜂鸣器状态


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
        delay_ms(200);  // 刷新周期
    }
}

