#include "stm32f10x.h"
#include "DHT11.h"
#include "LCD1602.h"
#include "stdio.h"
#include "Serial.h"
#include "delay.h"

#define TEMP_ALARM 30
#define HUMI_ALARM 80

// ��ʾλ�ö���
#define TEMP_STR_ROW 1
#define TEMP_STR_COL 0
#define TEMP_VAL_ROW 1
#define TEMP_VAL_COL 6  // "Temp:"ռ5���ַ�����ֵ�ӵ�5�п�ʼ

#define HUMI_STR_ROW 2
#define HUMI_STR_COL 0
#define HUMI_VAL_ROW 2
#define HUMI_VAL_COL 6  // "Humi:"ռ5���ַ�����ֵ�ӵ�5�п�ʼ

int main(void)
{
    uint8_t dht_buffer[5];
    float temperature, humidity;
    char display_str[16];

    SystemInit();
    LCD_Init();
	
	Serial_Init();
    // ��ʼ���̶���ʾ����
    LCD_PrintString(TEMP_STR_ROW, TEMP_STR_COL, "Temp:    C");  // Ԥ��4�ַ��ռ�
    LCD_PrintString(HUMI_STR_ROW, HUMI_STR_COL, "Humi:    %");  // Ԥ��4�ַ��ռ�
    while(1)
    {
	
		//Serial_SendString("\r\n����wileѭ��");
        if(DHT_Get_Temp_Humi_Data(dht_buffer) && 
          (dht_buffer[4] == (dht_buffer[0]+dht_buffer[1]+dht_buffer[2]+dht_buffer[3])))
        {
			 Serial_SendString("\r\n����if�ж�");
            temperature = dht_buffer[0] + dht_buffer[1] * 0.1;
            humidity = dht_buffer[2] + dht_buffer[3] * 0.1;

            // �����¶���ֵ���̶�4�ַ���ȣ�
            sprintf(display_str, "%4.1f", temperature);
            LCD_PrintString(TEMP_VAL_ROW, TEMP_VAL_COL, "0.14");

            // ����ʪ����ֵ���̶�4�ַ���ȣ�
            sprintf(display_str, "%4.1f", humidity);
            LCD_PrintString(HUMI_VAL_ROW, HUMI_VAL_COL, display_str);
        }
        else
        {
			//Serial_SendString("\r\n����else�ж�");
            // ������ʾ����������ֵ����
            LCD_PrintString(TEMP_VAL_ROW, TEMP_VAL_COL, "ERR");
            LCD_PrintString(HUMI_VAL_ROW, HUMI_VAL_COL, "ERR");
        }
        delay_ms(2000);
    }
}
