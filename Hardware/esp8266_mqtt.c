//#include "esp8266_mqtt.h"
//#include <string.h>
//#include <stdio.h>

//// ȫ�ֽ��ջ�����
//volatile char esp_rx_buffer[256] = {0};
//volatile uint8_t esp_rx_index = 0;

//// UART�����жϷ�����
//void USART2_IRQHandler(void) {
//    if (USART_GetITStatus(ESP_USART, USART_IT_RXNE) != RESET) {
//        char ch = USART_ReceiveData(ESP_USART);
//        esp_rx_buffer[esp_rx_index++] = ch;
//        if (esp_rx_index >= sizeof(esp_rx_buffer)) esp_rx_index = 0;
//    }
//}

//// ��ʼ��ESP8266ͨ��
//void ESP8266_Init(uint32_t baudrate) {
//    // ��ʼ��UART2��PA2-TX, PA3-RX��
//    USART_InitTypeDef uart_init;
//    GPIO_InitTypeDef gpio_init;

//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

//    // ����TX����
//    gpio_init.GPIO_Pin = ESP_TX_PIN;
//    gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
//    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init(ESP_GPIO_PORT, &gpio_init);

//    // ����RX����
//    gpio_init.GPIO_Pin = ESP_RX_PIN;
//    gpio_init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//    GPIO_Init(ESP_GPIO_PORT, &gpio_init);

//    // ����UART����
//    uart_init.USART_BaudRate = baudrate;
//    uart_init.USART_WordLength = USART_WordLength_8b;
//    uart_init.USART_StopBits = USART_StopBits_1;
//    uart_init.USART_Parity = USART_Parity_No;
//    uart_init.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
//    USART_Init(ESP_USART, &uart_init);
//    USART_ITConfig(ESP_USART, USART_IT_RXNE, ENABLE);
//    USART_Cmd(ESP_USART, ENABLE);

//    // �����ж�
//    NVIC_InitTypeDef nvic_init;
//    nvic_init.NVIC_IRQChannel = USART2_IRQn;
//    nvic_init.NVIC_IRQChannelPreemptionPriority = 0;
//    nvic_init.NVIC_IRQChannelSubPriority = 0;
//    nvic_init.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&nvic_init);
//}

//// ����ATָ��ȴ���Ӧ
//uint8_t ESP8266_SendCmd(const char *cmd, const char *ack, uint16_t timeout) {
//    memset((void*)esp_rx_buffer, 0, sizeof(esp_rx_buffer));
//    esp_rx_index = 0;
//    USART_SendData(ESP_USART, (uint8_t*)cmd, strlen(cmd));

//    while (timeout--) {
//        if (strstr((char*)esp_rx_buffer, ack) != NULL) return 1;
//        Delay_ms(1);
//    }
//    return 0; // ��ʱ��δ�ҵ�Ӧ��
//}

//// ���Ӱ�����MQTT
//void ESP8266_Connect_Aliyun(void) {
//    // 1. ����WiFiģʽ
//    ESP8266_SendCmd("AT+CWMODE=1\r\n", "OK", 1000);

//    // 2. ����WiFi���滻SSID�����룩
//    char wifi_cmd[64];
//    sprintf(wifi_cmd, "AT+CWJAP=\"Your_SSID\",\"Your_Password\"\r\n");
//    ESP8266_SendCmd(wifi_cmd, "OK", 5000);

//    // 3. ����MQTT���������ڰ�������Ԫ�����ɣ�
//    char mqtt_conn[256];
//    sprintf(mqtt_conn, 
//        "AT+MQTTUSERCFG=0,1,\"%s&%s\",\"%s&%s\",\"%s\",0,0,\"\"\r\n",
//        PRODUCT_KEY, DEVICE_NAME, PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET
//    );
//    ESP8266_SendCmd(mqtt_conn, "OK", 1000);

//    // 4. ����MQTT������
//    sprintf(mqtt_conn, 
//        "AT+MQTTCONN=0,\"%s.iot-as-mqtt.cn-shanghai.aliyuncs.com\",1883,1\r\n",
//        PRODUCT_KEY
//    );
//    ESP8266_SendCmd(mqtt_conn, "OK", 5000);
//}

//// ����MQTT��Ϣ
//void ESP8266_MQTT_Publish(const char *topic, const char *data) {
//    char cmd[128];
//    sprintf(cmd, "AT+MQTTPUB=0,\"%s\",\"%s\",1,0\r\n", topic, data);
//    ESP8266_SendCmd(cmd, "OK", 2000);
//}
