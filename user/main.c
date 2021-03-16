//////////////////////////////////////////////////////////////////////////////////
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//中景园电子
//店铺地址：http://shop73023976.taobao.com/?spm=2013.1.0.0.M4PqC2
//
//  文 件 名   : main.c
//  版 本 号   : v2.0
//  作    者   : HuangKai
//  生成日期   : 2017-0101
//  最近修改   :
//  功能描述   : OLED 4接口演示例程(STM32F103ZET6系列)
//              说明:
//              ----------------------------------------------------------------
//              GND    	电源地
//              VCC  	接5V或3.3v电源
//              SCK   	接PD6（SCL）
//              SDA   	接PB7（SDA）
//              RES  	接PD7
//              DC   	接PA4
//              CS1   	接PC6
//              FSO   	接PG15
//              CS2   	接PC8
//				EN	 	接PC7
//              ----------------------------------------------------------------
// 修改历史   :
// 日    期   :
// 作    者   : HuangKai
// 修改内容   : 创建文件
//版权所有，盗版必究。
//Copyright(C) 中景园电子2014/3/16
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





