#include "DHT11.h"
#include "Serial.h"

/**
  * @函数  DHT11_GPIO初始化函数
  * @参数  Mode：指定输入或输出模式
  * @返回值  None
  */

  // 精确延时函数（72MHz主频）
static void delay_us(uint32_t us) {
	us = us / 2;
    us = us * 9; // 72MHz / 8 = 9MHz
    while(us--) {
        __NOP(); __NOP(); __NOP(); __NOP(); 
        __NOP(); __NOP(); __NOP(); __NOP();
    }
}

static void delay_ms(uint32_t ms) {
    while(ms--) {
        delay_us(1000);
    }
}

void DHT_GPIO_Init(GPIOMode_TypeDef Mode)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(DHT_RCC_PORT,ENABLE);  //打开对应引脚时钟
	
	GPIO_SetBits(DHT_GPIO_PORT,DHT_GPIO_PIN);
	
	GPIO_InitStructure.GPIO_Mode = Mode;  //通过参数形式来控制GPIO的模式
	GPIO_InitStructure.GPIO_Pin = DHT_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(DHT_GPIO_PORT,&GPIO_InitStructure);
}
 

/**
  * @函数  DHT11模块起始信号函数
  * @参数  None
  * @返回值 1或0，标志起动信号成功与否
  */
uint8_t DHT_Start(void)
{
	//Serial_SendString("\r\n进入温湿度模块start函数");
	DHT_GPIO_Init(GPIO_Mode_Out_PP);							//推挽输出模式
	//Serial_SendString("\r\n完成温湿度模块init函数");

	GPIO_ResetBits(DHT_GPIO_PORT,DHT_GPIO_PIN);		//主机控制单总线输出20ms低电平
	delay_ms(20);
	
	//Serial_SendByte(GPIO_ReadInputDataBit(DHT_GPIO_PORT,DHT_GPIO_PIN));
	
	GPIO_SetBits(DHT_GPIO_PORT,DHT_GPIO_PIN);       //主机输出高电平后主机释放总线
	delay_us(30);
	
	//Serial_SendByte(GPIO_ReadInputDataBit(DHT_GPIO_PORT,DHT_GPIO_PIN));
	
	DHT_GPIO_Init(GPIO_Mode_IN_FLOATING);					//输出高电平后主机释放总线，配置位浮空输入模式
	delay_us(30);                                           //延时20~40us接收从机的应答信号
	//Serial_SendString("\r\n");
	//Serial_SendByte(GPIO_ReadInputDataBit(DHT_GPIO_PORT,DHT_GPIO_PIN));
	
	if(!GPIO_ReadInputDataBit(DHT_GPIO_PORT,DHT_GPIO_PIN))   //判断从机是否进入应答模式
	{
		while(!GPIO_ReadInputDataBit(DHT_GPIO_PORT,DHT_GPIO_PIN));   //while循环等待从机低电平应答  大概低电平时间持续80us（具体看数据手册）
		//while(GPIO_ReadInputDataBit(DHT_GPIO_PORT,DHT_GPIO_PIN));    //while循环等待从机高电平应答   大概高电平时间87us（具体看手册）
		//Serial_SendString("\r\n完成温湿度模块应答成功");
		return 1;    //返回1表示应答成功
	}
	//Serial_SendString("\r\n完成温湿度模块应答失败");
	return 0;   //返回0表示应答失败
}
 
/**
  * @函数   接收DHT11发送来8位的数据
  * @参数  None
  * @返回值 返回接收到的8位数据
  数据手册：数据位的传输：数据位为“0”时，DHT11先输出50~58μs的低电平随后输出23~27μs的高电平；数据位
  为“1”时，DHT11首先输出50~58μs的低电平随后输出68~74μs的高电平。
  判断数据位为1或0的方法：先whlie等待高电平时间结束，然后延时28us，即28us后再读取读取总线引脚，若还为高电平，则数据为位1，否则为0
  */
uint8_t DHT_Get_Byte_Data(void) {
    uint8_t temp = 0;  // 初始化变量为0
    for (int i = 0; i < 8; i++) {
        temp <<= 1;    // 左移腾出最低位
        
        // 1. 等待低电平结束（50us低电平时隙开始）
        while (GPIO_ReadInputDataBit(DHT_GPIO_PORT, DHT_GPIO_PIN) == SET); // 等待总线释放为高电平
        while (GPIO_ReadInputDataBit(DHT_GPIO_PORT, DHT_GPIO_PIN) == RESET); // 等待低电平结束（高电平开始）
        
        // 2. 测量高电平持续时间区分0/1
        delay_us(40);  // 关键点：等待超过数字0的高电平时间（26-28us）
        if (GPIO_ReadInputDataBit(DHT_GPIO_PORT, DHT_GPIO_PIN) == SET) {
            temp |= 0x01;  // 高电平持续超过30us → 判定为1
            while (GPIO_ReadInputDataBit(DHT_GPIO_PORT, DHT_GPIO_PIN) == SET); // 等待高电平结束（进入下一bit的低电平）
        } 
        // 否则为0，无需额外等待（下一bit的低电平已开始）
    }
    return temp;
}
 
/**
  * @函数  获取DHT11的温度湿度数据
  * @参数  buffer[]：需要传入一个存储数据的数组
  * @返回值 返回数据校验是否正确 1：正确 0：失败
  */
uint8_t DHT_Get_Temp_Humi_Data(uint8_t buffer[])
{
	//Serial_SendString("\r\n进入温湿度模块");
	if(DHT_Start())  //if判断DHT11从机是否应答完毕
	{
		//Serial_SendString("\r\n采集温湿度数据");
		buffer[0] = DHT_Get_Byte_Data();  //读取温度的整数放入数组第一个元素
		buffer[1] = DHT_Get_Byte_Data();  //读取温度的小数放入数组第二个元素
		buffer[2] = DHT_Get_Byte_Data();  //读取湿度的整数放入数组第三个元素
		buffer[3] = DHT_Get_Byte_Data();  //读取温度的小数放入数组第四个元素
		buffer[4] = DHT_Get_Byte_Data();  //读取的校验数据放入数组的第五个元素，用来检测数据是否读取正确
		//Serial_SendArray(buffer,5);	
	}
	return (buffer[0]+buffer[1]+buffer[2]+buffer[3] == buffer[4]) ? 1 : 0;   //校验数据是否传输正确
}
