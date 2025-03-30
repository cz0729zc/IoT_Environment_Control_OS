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

// 在全局变量区添加
float g_temperature;
float g_humidity;
uint16_t g_noise;
float g_pm25;

/* 阈值 */
uint8_t g_temp_thres = 33;
uint8_t g_humi_thres = 80;
uint8_t g_noise_thres = 60;

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

// 在MQTT发送失败时进行重试
#define MAX_RETRY 3
uint8_t retry_count = 0;

void Upload_Sensor_Data() {
	char payload[256];
	
    uint8_t soil_status = SoilHumidity_Read();
    uint8_t beep_status = GPIO_ReadInputDataBit(BEEP_GPIO,BEEP_PIN); // 蜂鸣器状态
    
    // 构建JSON数据包
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

    // 发布MQTT消息
    MQTT_PublishQs0(P_TOPIC_NAME, payload, strlen(payload));
}

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
    Usart3_Init(115200);
    //USART_Init(USART3,115200);
    
    Key_Init();
    SoilHumidity_Init();
    BEEP_Init();

    // WIFI初始化
    WiFi_ResetIO_Init();
    MQTT_Buff_Init();
    AliIoT_Parameter_Init();

    // 定时器初始化
    Timer1_Init(49,7200);   //10Khz的计数频率，计数到49为5ms
    TIM4_Init(5000,7200);   //TIM4初始化，定时时间 5000 * 7200 * 1000/72000000 = 500ms
    
    // 初始显示
    LCD_PrintString(1, 0, "Loading System...");
    delay_ms(1000);
    LCD_Clear();

    // 初始化WiFi模块
    LCD_PrintString(1, 0, "AT Test...");
    if (WiFi_SendCmd("AT", 20) == 0) {
        Serial_Printf("\r\n->模块响应正常:%s", WiFi_RX_BUF);
        LCD_PrintString(2, 0, "AT OK");
    } else {
        Serial_Printf("\r\n->模块无响应！");
        LCD_PrintString(2, 0, "AT FAIL");
		delay_ms(500);
		LCD_PrintString(2, 0, "Please RST");
		while(!(WiFi_SendCmd("AT", 20) == 0));
    }

    delay_ms(500);
    LCD_Clear();
    
    LCD_PrintString(1, 0, "WIFI init...");
    Serial_Printf("准备设置STA模式\r\n");
    if(WiFi_SendCmd("AT+CWMODE=1",50)) {
        Serial_Printf("设置STA模式失败，准备重启\r\n");
    } else {
        Serial_Printf("设置STA模式成功\r\n");
    }	
    
    LCD_PrintString(1, 0, "WIFI Connect...");
    if (WiFi_JoinAP(30) == 0) {
        LCD_PrintString(2, 0, "WIFI OK");
        Serial_Printf("连接路由器成功！");
    } else {
        LCD_PrintString(2, 0, "WIFI FALL");
        Serial_Printf("连接路由器失败！");
    }
    delay_ms(500);
    LCD_Clear();

    while (1) {	

        if(Connect_flag == 1) {  		
			
            /* 处理发送缓冲区数据 */
            if(MQTT_TxDataOutPtr != MQTT_TxDataInPtr) {
                if((MQTT_TxDataOutPtr[2]==0x10) || 
                   ((MQTT_TxDataOutPtr[2]==0x82) && (ConnectPack_flag==1)) || 
                   (SubcribePack_flag==1)) {    
                    Serial_Printf("发送数据:0x%x\r\n", MQTT_TxDataOutPtr[2]);
                    MQTT_TxData(MQTT_TxDataOutPtr);
                    MQTT_TxDataOutPtr += BUFF_UNIT;
                    if(MQTT_TxDataOutPtr == MQTT_TxDataEndPtr)
                        MQTT_TxDataOutPtr = MQTT_TxDataBuf[0];
                } 
            }

            /* 处理接收缓冲区数据 */
            if(MQTT_RxDataOutPtr != MQTT_RxDataInPtr) {
                Serial_Printf("接收到数据:");
                if(MQTT_RxDataOutPtr[2] == 0x20) {             			
                    switch(MQTT_RxDataOutPtr[5]) {					
                        case 0x00 : 
                            Serial_Printf("CONNECT报文成功\r\n");
                            ConnectPack_flag = 1;
                            break;                                              
                        case 0x01 : 
                            Serial_Printf("连接已拒绝，不支持的协议版本，准备重启\r\n");
                            Connect_flag = 0;
                            break;
                        case 0x02 : 
                            Serial_Printf("连接已拒绝，不合格的客户端标识符，准备重启\r\n");
                            Connect_flag = 0;
                            break;
                        case 0x03 : 
                            Serial_Printf("连接已拒绝，服务端不可用，准备重启\r\n");
                            Connect_flag = 0;
                            break;
                        case 0x04 : 
                            Serial_Printf("连接已拒绝，无效的用户名或密码，准备重启\r\n");
                            Connect_flag = 0;
                            break;
                        case 0x05 : 
                            Serial_Printf("连接已拒绝，未授权，准备重启\r\n");
                            Connect_flag = 0;
                            break;		
                        default   : 
                            Serial_Printf("连接已拒绝，未知状态，准备重启\r\n");
                            Connect_flag = 0;
                            break;								
                    }				
                }
                else if(MQTT_RxDataOutPtr[2] == 0x90) { 
                    switch(MQTT_RxDataOutPtr[6]) {					
                        case 0x00 :
                        case 0x01 : 
                            Serial_Printf("订阅成功\r\n");
                            SubcribePack_flag = 1;
                            Ping_flag = 0;
                            TIM3_ENABLE_30S();
                            TIM2_Int_Init(5000,7199);
                            break;                                             
                        default   : 
                            Serial_Printf("订阅失败，准备重启\r\n");
                            Connect_flag = 0;
                            break;								
                    }					
                }
                else if(MQTT_RxDataOutPtr[2] == 0xD0) { 
                    Serial_Printf("PING报文回复\r\n");
                    if(Ping_flag == 1) {
                        Ping_flag = 0;
                    } else if(Ping_flag > 1) {
                        Ping_flag = 0;
                        TIM3_ENABLE_30S();
                    }				
                }	
                else if(MQTT_RxDataOutPtr[2] == 0x30) { 
                    Serial_Printf("服务器等级0推送\r\n");
                    MQTT_DealPushdata_Qs0(MQTT_RxDataOutPtr);
                    MQTT_RxDataOutPtr += BUFF_UNIT;
                    if(MQTT_RxDataOutPtr == MQTT_RxDataEndPtr)
                        MQTT_RxDataOutPtr = MQTT_RxDataBuf[0];
                }
                MQTT_RxDataOutPtr += BUFF_UNIT;
                if(MQTT_RxDataOutPtr == MQTT_RxDataEndPtr)
                    MQTT_RxDataOutPtr = MQTT_RxDataBuf[0];
            } // 结束接收缓冲区处理
        } 
        else {
            Serial_Printf("需要连接服务器\r\n");
            WiFi_RxCounter = 0;                        
            memset(WiFi_RX_BUF, 0, WiFi_RXBUFF_SIZE);
            if(WiFi_Connect_IoTServer() == 0) {   			     
                Serial_Printf("建立TCP连接成功\r\n");
                Connect_flag = 1;	                           
                WiFi_RxCounter = 0;                        
                memset(WiFi_RX_BUF, 0, WiFi_RXBUFF_SIZE);
                MQTT_Buff_ReInit();
                Serial_Printf("已成功上载所有控制参数!\r\n");
            }				
        }
		
		
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
                        case SEL_NOISE: g_noise_thres++; break;
                    }
                }
                break;

            case KEY_SUB:  // 减少选中参数的值
                if (g_state == STATE_THRESHOLD) {
                    switch (g_select) {
                        case SEL_TEMP: g_temp_thres--; break;
                        case SEL_HUMI: g_humi_thres--; break;
                        case SEL_NOISE: g_noise_thres--; break;
                    }
                }
                break;
        }

        // 根据状态显示不同内容
        switch (g_state) {
            case STATE_PM25: {
                // PM2.5显示界面
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
				
                // 显示土壤湿度状态
                if (SoilHumidity_Read() == Bit_SET) {
                    LCD_PrintString(2, 0, "Dry"); // 土壤干燥
                } else {
                    LCD_PrintString(2, 0, "Wet"); // 土壤湿润
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

                LCD_PrintString(2, 0, "Noi:  dB");
                sprintf(display_str, "%2d", g_noise_thres);
                LCD_PrintString(2, 5, display_str);

				// 显示选中箭头
				if (g_select == SEL_TEMP) {
					LCD_ClearChar(2, 9);
					LCD_PrintChar(1, 8, '<');  // 温度选中，箭头在 (1, 8)
				} else if (g_select == SEL_HUMI) {
					LCD_ClearChar(1, 8);
					LCD_PrintChar(1, 16, '<'); // 湿度选中，箭头在 (1, 16)
				} else if (g_select == SEL_NOISE) {
					LCD_ClearChar(1, 16);
					LCD_PrintChar(2, 9, '<');   // 噪声选中，箭头在 (2, 9)
					
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
				
				// 温度超过阈值时打开蜂鸣器
				if (temperature > g_temp_thres) {
					BEEP_Alarm(1); // 打开蜂鸣器
				} else {
					BEEP_Alarm(0); // 关闭蜂鸣器
				}
			
                // 显示噪声数据
                noise_adc = ConvertToDecibel(LM2904_ReadValue());			
				//noise_adc = LM2904_ReadValue();
				g_noise = noise_adc;
                sprintf(display_str, "%3d", noise_adc);
                LCD_PrintString(NOISE_STR_ROW, 7, display_str);
                break;
				
				
            }
        }
		
		// 定时上传传感器数据
		if(Upload_TimeOK == 1) {
			Upload_TimeOK = 0;
			if(Connect_flag == 1) { // 确保连接正常
				Upload_Sensor_Data();
			}
		}
		
        delay_ms(200);  // 刷新周期
    }
}