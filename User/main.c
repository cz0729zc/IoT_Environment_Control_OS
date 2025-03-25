#include "stm32f10x.h"
#include "DHT11.h"
#include "LCD1602.h"
#include "stdio.h"
#include "Serial.h"
#include "delay.h"

#define TEMP_ALARM 30
#define HUMI_ALARM 80

// 显示位置定义
#define TEMP_STR_ROW 1
#define TEMP_STR_COL 0
#define TEMP_VAL_ROW 1
#define TEMP_VAL_COL 6  // "Temp:"占5个字符，数值从第5列开始

#define HUMI_STR_ROW 2
#define HUMI_STR_COL 0
#define HUMI_VAL_ROW 2
#define HUMI_VAL_COL 6  // "Humi:"占5个字符，数值从第5列开始

int main(void)
{
    uint8_t dht_buffer[5];
    float temperature, humidity;
    char display_str[16];

    SystemInit();
    LCD_Init();
	
	Serial_Init();
    // 初始化固定显示内容
    LCD_PrintString(TEMP_STR_ROW, TEMP_STR_COL, "Temp:    C");  // 预留4字符空间
    LCD_PrintString(HUMI_STR_ROW, HUMI_STR_COL, "Humi:    %");  // 预留4字符空间
    while(1)
    {
	
		//Serial_SendString("\r\n进入wile循环");
        if(DHT_Get_Temp_Humi_Data(dht_buffer) && 
          (dht_buffer[4] == (dht_buffer[0]+dht_buffer[1]+dht_buffer[2]+dht_buffer[3])))
        {
			 Serial_SendString("\r\n进入if判断");
            temperature = dht_buffer[0] + dht_buffer[1] * 0.1;
            humidity = dht_buffer[2] + dht_buffer[3] * 0.1;

            // 更新温度数值（固定4字符宽度）
            sprintf(display_str, "%4.1f", temperature);
            LCD_PrintString(TEMP_VAL_ROW, TEMP_VAL_COL, "0.14");

            // 更新湿度数值（固定4字符宽度）
            sprintf(display_str, "%4.1f", humidity);
            LCD_PrintString(HUMI_VAL_ROW, HUMI_VAL_COL, display_str);
        }
        else
        {
			//Serial_SendString("\r\n进入else判断");
            // 错误提示（仅覆盖数值区域）
            LCD_PrintString(TEMP_VAL_ROW, TEMP_VAL_COL, "ERR");
            LCD_PrintString(HUMI_VAL_ROW, HUMI_VAL_COL, "ERR");
        }
        delay_ms(2000);
    }
}
