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

/* 系统状态定义 */

typedef enum {
    STATE_THRESHOLD = 0,    // 阈值设置界面
    STATE_PM25,              // PM2.5显示界面
    STATE_REALTIME           // 实时数据显示界面
} SystemState;

/* 阈值设置界面中的选中项 */
typedef enum {
    SEL_TEMP = 0,   // 选中温度
    SEL_HUMI,       // 选中湿度
    SEL_NOISE       // 选中噪声
} SelectState;

/* 全局变量 */
SystemState g_state = STATE_THRESHOLD;  // 初始状态为阈值设置
SelectState g_select = SEL_TEMP;        // 初始选中温度

/* 阈值 */
uint8_t g_temp_thres = 33;
uint8_t g_humi_thres = 80;
uint8_t g_noise_thres = 60;

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
    Serial_Init();
    Serial3_Init();
    //PM25_Init();
    Key_Init();
	SoilHumidity_Init();
	BEEP_Init();

    WiFi_ResetIO_Init();
    MQTT_Buff_Init();
    AliIoT_Parameter_Init();

    // 初始显示
    LCD_PrintString(1, 0, "Loading System...");
	
    delay_ms(1000);
    LCD_Clear();
	

    // 初始化WiFi模块
    LCD_PrintString(1, 0, "AT Test...");
    
    if (WiFi_SendCmd("AT+RST", 20) == 0) {
        Serial_Printf("\r\n-> 模块响应正常: %s", WiFi_RX_BUF);
        LCD_PrintString(2, 0, "AT OK");
    } else {
        Serial_Printf("\r\n-> 模块无响应！");
        LCD_PrintString(2, 0, "AT FAIL");
    }
    delay_ms(500);
    LCD_Clear();
    
//    LCD_PrintString(1, 0, "WIFI init...");
//    Serial_Printf("准备设置STA模式\r\n");                  //串口提示数据
//    if(WiFi_SendCmd("AT+CWMODE=1",50)){                //设置STA模式，100ms超时单位，总计5s超时时间
//        Serial_Printf("设置STA模式失败，准备重启\r\n");    //返回非0值，进入if，串口提示数据
//    }else {
//        Serial_Printf("设置STA模式成功\r\n");            //串口提示数据
//    }	
//    
//    LCD_PrintString(1, 0, "WIFI Connect...");
//    if (WiFi_JoinAP(30) == 0) {
//        LCD_PrintString(2, 0, "WIFI OK");
//        Serial_Printf("连接路由器成功！");
//    } else {
//        LCD_PrintString(2, 0, "WIFI FALL");
//        Serial_Printf("连接路由器失败！");
//    }

//    delay_ms(500);
//    LCD_Clear();

//    LCD_PrintString(1, 0, "MQTT Connect...");
//    if (WiFi_Connect_IoTServer() == 0) {
//        LCD_PrintString(2, 0, "WIFI OK");
//        Serial_Printf("连接MQTT服务器成功！");
//    } else {
//        LCD_PrintString(2, 0, "WIFI FAIL");
//        Serial_Printf("连接MQTT服务器失败！");
//    }
    while (1) {
		//Serial3_SendByte(0xAA);
		//delay_ms(10);
		//Serial3_Printf("\r\n usart3 test");
		//Serial3_SendByte(0xCC);
		
		
        // 测试1：发送AT指令测试模块响应
        //Serial_Printf("\r\n[1] 发送AT指令测试...\r\n");	
			
		
//        // 按键处理
//        Key_Value key = Key_Scan();

//        switch (key) {
//            case KEY_UP:  // 切换界面
//                g_state = (g_state + 1) % 3;
//                LCD_Clear();
//                break;

//            case KEY_DOWN:  // 切换选中项（仅在阈值设置界面有效）
//                if (g_state == STATE_THRESHOLD) {
//                    g_select = (g_select + 1) % 3;
//                }
//                break;

//            case KEY_ADD:  // 增加选中参数的值
//                if (g_state == STATE_THRESHOLD) {
//                    switch (g_select) {
//                        case SEL_TEMP: g_temp_thres++; break;
//                        case SEL_HUMI: g_humi_thres++; break;
//                        case SEL_NOISE: g_noise_thres++; break;
//                    }
//                }
//                break;

//            case KEY_SUB:  // 减少选中参数的值
//                if (g_state == STATE_THRESHOLD) {
//                    switch (g_select) {
//                        case SEL_TEMP: g_temp_thres--; break;
//                        case SEL_HUMI: g_humi_thres--; break;
//                        case SEL_NOISE: g_noise_thres--; break;
//                    }
//                }
//                break;
//        }

//        // 根据状态显示不同内容
//        switch (g_state) {
//            case STATE_PM25: {
//                // PM2.5显示界面
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
//                // 显示土壤湿度状态
//                if (SoilHumidity_Read() == Bit_SET) {
//                    LCD_PrintString(2, 0, "Dry"); // 土壤干燥
//                } else {
//                    LCD_PrintString(2, 0, "Wet"); // 土壤湿润
//                }
//                break;
//            }

//            case STATE_THRESHOLD: {
//                // 阈值设置界面
//                LCD_PrintString(1, 0, "Tem:  C");
//                sprintf(display_str, "%2d", g_temp_thres);
//                LCD_PrintString(1, 5, display_str);

//                LCD_PrintString(1, 8, " Hum:  %");
//                sprintf(display_str, "%2d", g_humi_thres);
//                LCD_PrintString(1, 13, display_str);

//                LCD_PrintString(2, 0, "Noi:  dB");
//                sprintf(display_str, "%2d", g_noise_thres);
//                LCD_PrintString(2, 5, display_str);

//				// 显示选中箭头
//				if (g_select == SEL_TEMP) {
//					LCD_ClearChar(2, 9);
//					LCD_PrintChar(1, 8, '<');  // 温度选中，箭头在 (1, 8)
//				} else if (g_select == SEL_HUMI) {
//					LCD_ClearChar(1, 8);
//					LCD_PrintChar(1, 16, '<'); // 湿度选中，箭头在 (1, 16)
//				} else if (g_select == SEL_NOISE) {
//					LCD_ClearChar(1, 16);
//					LCD_PrintChar(2, 9, '<');   // 噪声选中，箭头在 (2, 9)
//					
//				}
//                break;
//            }

//            case STATE_REALTIME: {
//                // 实时数据显示界面
//                if (DHT_Get_Temp_Humi_Data(dht_buffer)) {
//                    LCD_PrintString(1, 0, "T:    C");
//                    LCD_PrintString(1, 10, "H:    %");
//                    LCD_PrintString(2, 0, "Noi:    dB");

//                    temperature = dht_buffer[2] + dht_buffer[3] * 0.1;
//                    humidity = dht_buffer[0] + dht_buffer[1] * 0.1;

//                    // 更新温度数值
//                    sprintf(display_str, "%4.1f", temperature);
//                    LCD_PrintString(TEMP_VAL_ROW, TEMP_VAL_COL, display_str);

//                    // 更新湿度数值
//                    sprintf(display_str, "%3.1f", humidity);
//                    LCD_PrintString(HUMI_VAL_ROW, HUMI_VAL_COL, display_str);
//                } else {
//                    LCD_PrintString(TEMP_VAL_ROW, TEMP_VAL_COL, "ERR");
//                    LCD_PrintString(HUMI_VAL_ROW, HUMI_VAL_COL, "ERR");
//                }
//				
//				// 温度超过阈值时打开蜂鸣器
//				if (temperature > g_temp_thres) {
//					BEEP_Alarm(1); // 打开蜂鸣器
//				} else {
//					BEEP_Alarm(0); // 关闭蜂鸣器
//				}
//			
//                // 显示噪声数据
//                noise_adc = LM2904_ReadValue();
//                sprintf(display_str, "%3d", noise_adc);
//                LCD_PrintString(NOISE_STR_ROW, 5, display_str);
//                break;
//				
//            }
//        }

//        delay_ms(200);  // 刷新周期
    }
}