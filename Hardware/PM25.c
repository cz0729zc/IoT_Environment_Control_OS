#include "PM25.h"
#include "string.h"
#include "serial.h"

static PM25_Data current_data;
static uint8_t rx_buffer[DATA_LENGTH];
static uint8_t rx_index = 0;
static uint8_t frame_started = 0;

//#define DEBUG_PM25_PROTOCOL 0

// 校验和计算（修正后的实现）
uint8_t FucCheckSum(uint8_t *buf, uint8_t len) {
    uint8_t temp = 0;
    for(uint8_t j = 1; j < len-1; j++) {
        temp += buf[j];
    }
    return (uint8_t)(~temp + 1);
}


// 数据解析函数
static void ParsePM25Data(uint8_t *data) {
    // 校验帧头和校验和
    if(data[0] != FRAME_HEADER) {
        Serial_Printf("\r\n帧头错误");
        current_data.data_valid = 0;
        return;
    }
    
    // 验证校验和
    uint8_t checksum = FucCheckSum(data, DATA_LENGTH);
    if(checksum != data[CHECKSUM_POS]) {
        Serial_Printf("\r\n校验和错误");
        current_data.data_valid = 0;
        return;
    }

    // 解析占空比
//	for(int i=0;i<8;i++)
////		Serial_SendByte(data[i]);
//	Serial_SendByte(data[3]);
//	Serial_SendByte(data[4]);
	
    uint8_t hex_integer  = data[3];
    uint8_t hex_decimal  = data[4];
	//Serial_Printf("\r\n hex_integer:%#X",hex_integer);
	//Serial_Printf("\r\n hex_decimal:%#X",hex_decimal);
	
	//Serial_Printf("\r\n hex_integer:%d",hex_integer);
	//Serial_Printf("\r\n hex_decimal:%d",hex_decimal);
	
	
    current_data.duty_cycle = (int)hex_integer + (int)hex_decimal/100.0f;
	//Serial_Printf("\r\n duty_cycle:%f",current_data.duty_cycle);
    // 计算PM2.5浓度（根据传感器特性调整系数）
    current_data.pm25_value = (current_data.duty_cycle - 0.5) * 400 / 49.5;

    //Serial_Printf("\r\n pm25_value: %04f\n", current_data.pm25_value);
    //输出限幅保护
    if(current_data.pm25_value < 0) 
        current_data.pm25_value = 0;
    else if(current_data.pm25_value > 400) 
        current_data.pm25_value = 400;

    current_data.data_valid = 1;
}

// 接收数据处理（需在串口中断中调用）
void PM25_ReceiveHandler(uint8_t data) {
    static uint8_t frame_started = 0;
    
    //Serial_Printf("\r\n进入数据处理\r\n");
    //Serial_SendByte(data);
#ifdef DEBUG_PM25_PROTOCOL  // 调试模式开关
    Serial_Printf("[RAW] 0x%02X\n", data);  // 实时打印每个原始字节
#endif

    if(!frame_started) {
        if(data == FRAME_HEADER) {
#ifdef DEBUG_PM25_PROTOCOL
            Serial_Printf("\n--- Frame Start ---\n");
#endif
            rx_index = 0;
            rx_buffer[rx_index++] = data;
            frame_started = 1;
        }
    } else {
        rx_buffer[rx_index++] = data;
        
        if(rx_index >= DATA_LENGTH) {
#ifdef DEBUG_PM25_PROTOCOL
            Serial_Printf("Full Frame: ");
            for(int i=0; i<DATA_LENGTH; i++){
                Serial_Printf("%02X ", rx_buffer[i]);
            }
            Serial_Printf("\n");
#endif
            ParsePM25Data(rx_buffer);
            frame_started = 0;
        }
    }
}

// 获取当前数据
PM25_Data PM25_GetCurrentData(void) {
	//Serial_Printf("获取当前PM2.5数据");
    return current_data;
}

// 模块初始化
void PM25_Init(void) {
    memset(&current_data, 0, sizeof(PM25_Data));
}
