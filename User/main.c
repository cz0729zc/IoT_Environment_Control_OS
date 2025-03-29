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

/* 阈值 */
uint8_t g_temp_thres = 33;
uint8_t g_humi_thres = 80;
uint8_t g_noise_thres = 60;

uint8_t Second = 0;
uint8_t Minute = 0;
uint8_t Hour = 0;
uint8_t Upload_TimeOK = 0;  // 上载数据标志位

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
        delay_ms(200);
        TIM3_ENABLE_30S();
		
		
        // Serial_Printf("发送out值： MQTT_TxDataOutPtr :%d\r\n",MQTT_TxDataOutPtr);
        // Serial_Printf("发送in值：  MQTT_TxDataInPtr  :%d\r\n\r\n",MQTT_TxDataInPtr);
        // Serial_Printf("接收out值： MQTT_RxDataOutPtr :%d\r\n",MQTT_RxDataOutPtr);
        // Serial_Printf("接收in值：  MQTT_RxDataInPtr  :%d\r\n",MQTT_RxDataInPtr);
       
        //Serial_Printf("接收值：  MQTT_RxDataOutPtr[2]  :%d\r\n\r\n",MQTT_RxDataOutPtr[2]);
        if(Connect_flag == 1) {  
			
//			if (ConnectPack_flag == 1) {
//				MQTT_Subscribe(S_TOPIC_NAME, 0);  // 订阅主题，QoS=0
//				Serial_Printf("已订阅主题：%s\r\n", S_TOPIC_NAME);
//			}			
			
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

        // 其他传感器和显示代码（已注释部分）
        // ... [原有注释的传感器和显示代码]
    }
}