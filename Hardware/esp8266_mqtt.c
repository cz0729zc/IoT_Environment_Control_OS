//#include "esp8266_mqtt.h"
//#include <string.h>
//#include <stdio.h>

//// 全局接收缓冲区
//volatile char esp_rx_buffer[256] = {0};
//volatile uint8_t esp_rx_index = 0;

//// UART接收中断服务函数
//void USART2_IRQHandler(void) {
//    if (USART_GetITStatus(ESP_USART, USART_IT_RXNE) != RESET) {
//        char ch = USART_ReceiveData(ESP_USART);
//        esp_rx_buffer[esp_rx_index++] = ch;
//        if (esp_rx_index >= sizeof(esp_rx_buffer)) esp_rx_index = 0;
//    }
//}

//// 初始化ESP8266通信
//void ESP8266_Init(uint32_t baudrate) {
//    // 初始化UART2（PA2-TX, PA3-RX）
//    USART_InitTypeDef uart_init;
//    GPIO_InitTypeDef gpio_init;

//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

//    // 配置TX引脚
//    gpio_init.GPIO_Pin = ESP_TX_PIN;
//    gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
//    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init(ESP_GPIO_PORT, &gpio_init);

//    // 配置RX引脚
//    gpio_init.GPIO_Pin = ESP_RX_PIN;
//    gpio_init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//    GPIO_Init(ESP_GPIO_PORT, &gpio_init);

//    // 配置UART参数
//    uart_init.USART_BaudRate = baudrate;
//    uart_init.USART_WordLength = USART_WordLength_8b;
//    uart_init.USART_StopBits = USART_StopBits_1;
//    uart_init.USART_Parity = USART_Parity_No;
//    uart_init.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
//    USART_Init(ESP_USART, &uart_init);
//    USART_ITConfig(ESP_USART, USART_IT_RXNE, ENABLE);
//    USART_Cmd(ESP_USART, ENABLE);

//    // 启用中断
//    NVIC_InitTypeDef nvic_init;
//    nvic_init.NVIC_IRQChannel = USART2_IRQn;
//    nvic_init.NVIC_IRQChannelPreemptionPriority = 0;
//    nvic_init.NVIC_IRQChannelSubPriority = 0;
//    nvic_init.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&nvic_init);
//}

//// 发送AT指令并等待响应
//uint8_t ESP8266_SendCmd(const char *cmd, const char *ack, uint16_t timeout) {
//    memset((void*)esp_rx_buffer, 0, sizeof(esp_rx_buffer));
//    esp_rx_index = 0;
//    USART_SendData(ESP_USART, (uint8_t*)cmd, strlen(cmd));

//    while (timeout--) {
//        if (strstr((char*)esp_rx_buffer, ack) != NULL) return 1;
//        Delay_ms(1);
//    }
//    return 0; // 超时或未找到应答
//}

//// 连接阿里云MQTT
//void ESP8266_Connect_Aliyun(void) {
//    // 1. 设置WiFi模式
//    ESP8266_SendCmd("AT+CWMODE=1\r\n", "OK", 1000);

//    // 2. 连接WiFi（替换SSID和密码）
//    char wifi_cmd[64];
//    sprintf(wifi_cmd, "AT+CWJAP=\"Your_SSID\",\"Your_Password\"\r\n");
//    ESP8266_SendCmd(wifi_cmd, "OK", 5000);

//    // 3. 配置MQTT参数（基于阿里云三元组生成）
//    char mqtt_conn[256];
//    sprintf(mqtt_conn, 
//        "AT+MQTTUSERCFG=0,1,\"%s&%s\",\"%s&%s\",\"%s\",0,0,\"\"\r\n",
//        PRODUCT_KEY, DEVICE_NAME, PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET
//    );
//    ESP8266_SendCmd(mqtt_conn, "OK", 1000);

//    // 4. 连接MQTT服务器
//    sprintf(mqtt_conn, 
//        "AT+MQTTCONN=0,\"%s.iot-as-mqtt.cn-shanghai.aliyuncs.com\",1883,1\r\n",
//        PRODUCT_KEY
//    );
//    ESP8266_SendCmd(mqtt_conn, "OK", 5000);
//}

//// 发布MQTT消息
//void ESP8266_MQTT_Publish(const char *topic, const char *data) {
//    char cmd[128];
//    sprintf(cmd, "AT+MQTTPUB=0,\"%s\",\"%s\",1,0\r\n", topic, data);
//    ESP8266_SendCmd(cmd, "OK", 2000);
//}
