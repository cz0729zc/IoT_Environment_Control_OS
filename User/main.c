#include "stm32f10x.h"
#include "DHT11.h"
#include "LCD1602.h"
#include "stdio.h"
#include "Serial.h"
#include "delay.h"
#include "LM2904.h"
#include "PM25.h"

#define TEMP_ALARM 30  // �¶ȱ�����ֵ
#define HUMI_ALARM 80  // ʪ�ȱ�����ֵ

// ��ʾλ�ö���
#define TEMP_STR_ROW 1  
#define TEMP_STR_COL 0
#define TEMP_VAL_ROW 1
#define TEMP_VAL_COL 3  // "Temp:"ռ5���ַ�����ֵ�ӵ�5�п�ʼ

#define HUMI_STR_ROW 2
#define HUMI_STR_COL 0
#define HUMI_VAL_ROW 2
#define HUMI_VAL_COL 3  // "Humi:"ռ5���ַ�����ֵ�ӵ�5�п�ʼ

#define NOISE_STR_ROW 1 // ������ʾ�ڵ�1���Ҳ�
#define NOISE_VAL_ROW 0

#define PM25_STR_ROW 1
#define PM25_STR_COL 0
#define PM25_VAL_ROW 1
#define PM25_VAL_COL 6

#define COLUMN_OFFSET 9  // �Ҳ�������ʼ��

#define DEBUG_PM25_PROTOCOL //debugģʽ

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
    // ��ʼ���̶���ʾ����
    // LCD_PrintString(TEMP_STR_ROW, TEMP_STR_COL, "T:    C");  // Ԥ��4�ַ��ռ�
    // LCD_PrintString(HUMI_STR_ROW, HUMI_STR_COL, "H:    %");  // Ԥ��4�ַ��ռ�
    // LCD_PrintString(NOISE_STR_ROW, COLUMN_OFFSET, "N:   dB"); // ����������ǩ
	LCD_PrintString(PM25_STR_ROW, PM25_STR_COL, "PM2.5:   ug/m3");
    //Serial_Printf("\r\n ****************************************************************");
    while(1)
    {
        // if(DHT_Get_Temp_Humi_Data(dht_buffer) && 
        //   (dht_buffer[4] == (dht_buffer[0]+dht_buffer[1]+dht_buffer[2]+dht_buffer[3])))
        // {
		// 	 Serial_SendString("\r\n����if�ж�");
        //     humidity = dht_buffer[0] + dht_buffer[1] * 0.1;
        //     temperature = dht_buffer[2] + dht_buffer[3] * 0.1;

        //     // �����¶���ֵ���̶�4�ַ���ȣ�
        //     sprintf(display_str, "%4.1f", temperature);
        //     LCD_PrintString(TEMP_VAL_ROW, TEMP_VAL_COL, display_str);

        //     // ����ʪ����ֵ���̶�4�ַ���ȣ�
        //     sprintf(display_str, "%4.1f", humidity);
        //     LCD_PrintString(HUMI_VAL_ROW, HUMI_VAL_COL, display_str);
        // }
        // else
        // {
		// 	//Serial_SendString("\r\n����else�ж�");
        //     // ������ʾ����������ֵ����
        //     LCD_PrintString(TEMP_VAL_ROW, TEMP_VAL_COL, "ERR");
        //     LCD_PrintString(HUMI_VAL_ROW, HUMI_VAL_COL, "ERR");
        // }

        // /* ������ʾ���� */
        // noise_adc = ConvertToDecibel(LM2904_ReadValue());
        // sprintf(display_str, "%3d", noise_adc);
        // LCD_PrintString(NOISE_STR_ROW, COLUMN_OFFSET+2, display_str); // ��ֵ��ʾλ��

        PM25_Data pm_data = PM25_GetCurrentData();

        if(pm_data.data_valid) 
        {
            char lcd_line1[17];  // LCD1602ÿ��16�ַ�+������
            char lcd_line2[17];
            //Serial_Printf("pm25_value: %04f\n", pm_data.pm25_value);
            // ��һ����ʾPM2.5Ũ�ȣ�����λ��
            snprintf(lcd_line1, sizeof(lcd_line1), "PM2.5:%3.0fug/m3", pm_data.pm25_value);
			
			if(pm_data.pm25_value != 0)
			{
				LCD_Clear();        
				LCD_PrintString(1, 0, lcd_line1);				
			}
        
            pm_data.data_valid = 0;  // �����Ч��־
        }
		else{
			LCD_PrintString(PM25_VAL_ROW,PM25_VAL_COL+1,"err");
		}

        delay_ms(2000);
    }
}
