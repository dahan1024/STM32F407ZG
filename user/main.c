//////////////////////////////////////////////////////////////////////////////////
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//�о�԰����
//���̵�ַ��http://shop73023976.taobao.com/?spm=2013.1.0.0.M4PqC2
//
//  �� �� ��   : main.c
//  �� �� ��   : v2.0
//  ��    ��   : HuangKai
//  ��������   : 2017-0101
//  ����޸�   :
//  ��������   : OLED 4�ӿ���ʾ����(STM32F103ZET6ϵ��)
//              ˵��:
//              ----------------------------------------------------------------
//              GND    	��Դ��
//              VCC  	��5V��3.3v��Դ
//              SCK   	��PD6��SCL��
//              SDA   	��PB7��SDA��
//              RES  	��PD7
//              DC   	��PA4
//              CS1   	��PC6
//              FSO   	��PG15
//              CS2   	��PC8
//				EN	 	��PC7
//              ----------------------------------------------------------------
// �޸���ʷ   :
// ��    ��   :
// ��    ��   : HuangKai
// �޸�����   : �����ļ�
//��Ȩ���У�����ؾ���
//Copyright(C) �о�԰����2014/3/16
//All rights reserved
//******************************************************************************/

#include <stdio.h>

#include <cli_server.h>
#include <drv_delay.h>

#include "system.h"


int main(void)
{
	system_init();

	while (1)
	{
		if (cli_server_rx_state & 0x8000)	// rx completed
		{
			cli_server_rx_buf[cli_server_rx_state & 0x3fff] = '\0';
			cli_server_cmd_loop((char *)cli_server_rx_buf);
			cli_server_rx_state = 0x0000;
		}

	}
}





