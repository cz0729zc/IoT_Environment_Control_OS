PB10 ,PB11,PA15 默认为Jtag引脚要想复用为普通io口需要，关闭JTAG



使用串口发送数据的时候，每两个BIT之间需要加一定延时，		delay_us(5);
		delay_ms(2);

不然刷新率太高数据乱码或丢失



