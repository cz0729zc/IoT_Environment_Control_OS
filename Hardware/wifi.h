/*-------------------------------------------------*/
/*            ��γ����STM32ϵ�п�����              */
/*-------------------------------------------------*/
/*                                                 */
/*              ����Wifi���ܵ�ͷ�ļ�               */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __WIFI_H
#define __WIFI_H

#include "Serial3.h"	    //������Ҫ��ͷ�ļ�

#define RESET_IO(x)    GPIO_WriteBit(GPIOB, GPIO_Pin_7, (BitAction)x)  //PA4����WiFi�ĸ�λ

#define WiFi_printf       Serial3_Printf      // ����3���� WiFi
#define WiFi_RxCounter    Serial3_RxCounter   // ����3���� WiFi
#define WiFi_RX_BUF       Serial3_RxBuff      // ����3���� WiFi
#define WiFi_RXBUFF_SIZE  SERIAL3_RXBUFF_SIZE // ����3���� WiFi

//////////////////////#define SSID   "e-Lab_2.4G"   //"iQOO 5"//                  //·����SSID����
//////////////////////#define PASS   "dw199194"                 //·��������

//#define SSID   "e-Lab_2.4G"//"My_WIFI"   //"iQOO 5"//                  //·����SSID����
//#define PASS   "dw199194"//"88888888"                 //·��������

#define SSID   "My_WIFI"                  //·����SSID����
#define PASS   "88888888"                 //·��������

void WiFi_ResetIO_Init(void);
char WiFi_SendCmd(char *cmd, int timeout);
char WiFi_IO_Reset(int timeout);
char WiFi_CMD_Reset(int timeout);
char WiFi_JoinAP(int timeout);
char WiFi_Connect_Server(int timeout);
char WiFi_Smartconfig(int timeout);
char WiFi_WaitAP(int timeout);
char WiFi_Connect_IoTServer(void);


#endif


