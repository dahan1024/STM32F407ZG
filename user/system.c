/*
 * system.c
 *
 *  Created on: 2019年10月24日
 *      Author: yanglinjun
 */

#include <types.h>
#include <common.h>
#include <stm32f4xx.h>
#include <drv_delay.h>
#include <drv_fsmc_sram.h>
#include <cli_server.h>
#include <ad9543.h>
#include <eeprom.h>
#include <fpga.h>
#include <w25qxx.h>

static int32_t init_fpga(void);

void system_init(void)
{
	// Turn on peripherial clocks
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init();		//延时初始化
	fsmc_sram_init();

	// Initialize cli first, so we can use printf.
	cli_server_init();

	ad9543_init();
	eeprom_init();
	fpga_init();
	w25qxx_init();

	// initialize pac id
	delay_ms(1000);
	init_fpga();
}

static int32_t init_fpga(void)
{
	uint8_t pac_id;
	int32_t rc;

	if ((rc = eeprom_get_pac_id(&pac_id)) < 0)
	{
		error("Get pac id from eeprom failed\n");
		return rc;
	}

	if ((rc = fpga_set_pac_id(pac_id)) < 0)
	{
		error("Set pac id to fpga failed\n");
		return rc;
	}

	if ((rc = fpga_set_monitor_ver()) < 0)
	{
		error("Set monitor version to fpga failed\n");
		return rc;
	}

	return 0;
}





