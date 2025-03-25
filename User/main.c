#include "stm32f10x.h"
#include "DHT11.h"
#include "LCD1602.h"
#include "stdio.h"
#include "Serial.h"
#include "delay.h"
#include "LM2904.h"
#include "PM25.h"

#define TEMP_ALARM 30  // 温度报警阈值
#define HUMI_ALARM 80  // 湿度报警阈值

// 显示位置定义
#define TEMP_STR_ROW 1  
#define TEMP_STR_COL 0
#define TEMP_VAL_ROW 1
#define TEMP_VAL_COL 3  // "Temp:"占5个字符，数值从第5列开始

#define HUMI_STR_ROW 2
#define HUMI_STR_COL 0
#define HUMI_VAL_ROW 2
#define HUMI_VAL_COL 3  // "Humi:"占5个字符，数值从第5列开始

#define NOISE_STR_ROW 1 // 噪声显示在第1行右侧
#define NOISE_VAL_ROW 0

#define PM25_STR_ROW 1
#define PM25_STR_COL 0
#define PM25_VAL_ROW 1
#define PM25_VAL_COL 6

#define COLUMN_OFFSET 9  // 右侧区域起始列

#define DEBUG_PM25_PROTOCOL //debug模式

int main(void)
{
    uint8_t dht_buffer[5];
    float temperature, humidity;
    uint16_t noise_adc;
    char display_str[16];

    LCD_Init();
	LM2904_Init();
	Serial_Init();
    PM25_Init();
    // 初始化固定显示内容
    // LCD_PrintString(TEMP_STR_ROW, TEMP_STR_COL, "T:    C");  // 预留4字符空间
    // LCD_PrintString(HUMI_STR_ROW, HUMI_STR_COL, "H:    %");  // 预留4字符空间
    // LCD_PrintString(NOISE_STR_ROW, COLUMN_OFFSET, "N:   dB"); // 新增噪声标签
	LCD_PrintString(PM25_STR_ROW, PM25_STR_COL, "PM2.5:   ug/m3");
    //Serial_Printf("\r\n ****************************************************************");
    while(1)
    {
        // if(DHT_Get_Temp_Humi_Data(dht_buffer) && 
        //   (dht_buffer[4] == (dht_buffer[0]+dht_buffer[1]+dht_buffer[2]+dht_buffer[3])))
        // {
		// 	 Serial_SendString("\r\n进入if判断");
        //     humidity = dht_buffer[0] + dht_buffer[1] * 0.1;
        //     temperature = dht_buffer[2] + dht_buffer[3] * 0.1;

        //     // 更新温度数值（固定4字符宽度）
        //     sprintf(display_str, "%4.1f", temperature);
        //     LCD_PrintString(TEMP_VAL_ROW, TEMP_VAL_COL, display_str);

        //     // 更新湿度数值（固定4字符宽度）
        //     sprintf(display_str, "%4.1f", humidity);
        //     LCD_PrintString(HUMI_VAL_ROW, HUMI_VAL_COL, display_str);
        // }
        // else
        // {
		// 	//Serial_SendString("\r\n进入else判断");
        //     // 错误提示（仅覆盖数值区域）
        //     LCD_PrintString(TEMP_VAL_ROW, TEMP_VAL_COL, "ERR");
        //     LCD_PrintString(HUMI_VAL_ROW, HUMI_VAL_COL, "ERR");
        // }

        // /* 噪声显示处理 */
        // noise_adc = ConvertToDecibel(LM2904_ReadValue());
        // sprintf(display_str, "%3d", noise_adc);
        // LCD_PrintString(NOISE_STR_ROW, COLUMN_OFFSET+2, display_str); // 数值显示位置

        PM25_Data pm_data = PM25_GetCurrentData();

        if(pm_data.data_valid) 
        {
            char lcd_line1[17];  // LCD1602每行16字符+结束符
            char lcd_line2[17];
            //Serial_Printf("pm25_value: %04f\n", pm_data.pm25_value);
            // 第一行显示PM2.5浓度（带单位）
            snprintf(lcd_line1, sizeof(lcd_line1), "PM2.5:%3.0fug/m3", pm_data.pm25_value);
			
			if(pm_data.pm25_value != 0)
			{
				LCD_Clear();        
				LCD_PrintString(1, 0, lcd_line1);				
			}
        
            pm_data.data_valid = 0;  // 清除有效标志
        }
		else{
			LCD_PrintString(PM25_VAL_ROW,PM25_VAL_COL+1,"err");
		}

        delay_ms(2000);
    }
}
