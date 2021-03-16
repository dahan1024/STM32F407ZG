/*
 * lv2_ad9543.c
 *
 *  Created on: 2021Äê1ÔÂ11ÈÕ
 *      Author: cjl
 */

#include <stdio.h>
#include <stdlib.h>

#include <common.h>
#include <ad9543.h>
// comment out the following header file in the release version
#include <drv_spi.h>

#include "cli_server.h"

/*********************lv2_ad9543 begin*************************/
static int cli_spi3_disable_cs(Cmd_t *cmd_p, int argc, void **argv) {
	SPI3_CS0_DISABLE;

	return 0;
}

static int cli_spi3_enable_cs(Cmd_t *cmd_p, int argc, void **argv) {
	SPI3_CS0_ENABLE;

	return 0;
}

static int cli_spi3_transfer(Cmd_t *cmd_p, int argc, void **argv) {
	uint8_t data;
	char *ptr;

	data = (uint8_t)strtoul(argv[1], &ptr, 16);

	printf("spi3_transfer(0x%02x): succeeded. Return 0x%02x.\n", data, spi_transfer(SPI3, data));

	return 0;
}

static int cli_ad9543_write(Cmd_t *cmd_p, int argc, void **argv) {
	uint16_t reg_addr;
	uint8_t reg_val;
	char *ptr;

	if (argc != 3)
	{
		error("argument counter is not 3");

		return 0;
	}

	reg_addr = (uint16_t)strtoul(argv[1], &ptr, 16);
	reg_val = (uint8_t)strtoul(argv[2], &ptr, 16);

	if (ad9543_write(reg_addr, reg_val) < 0)
		printf("ad9543_write(0x%04x, 0x%02x): failed\n", reg_addr, reg_val);
	else
		printf("ad9543_write(0x%04x, 0x%02x): succeeded\n", reg_addr, reg_val);

	return 0;
}

static int cli_ad9543_read(Cmd_t *cmd_p, int argc, void **argv) {
	uint16_t reg_addr;
	uint8_t reg_val;
	char *ptr;

	if (argc != 2)
	{
		error("argument counter is not 3");

		return 0;
	}

	reg_addr = (uint16_t)strtoul(argv[1], &ptr, 16);

	if (ad9543_read(reg_addr, &reg_val) < 0)
		printf("ad9543_read(0x%04x): failed\n", reg_addr);
	else
		printf("ad9543_read(0x%04x): succeeded. reg_val is 0x%02x\n", reg_addr, reg_val);

	return 0;
}

static int lv2_ad9543_quit(Cmd_t *cmd_p, int argc, void **argv) {
	set_cur_cmd(&lv1);
	cli_server_cmd_loop(NULL);
	return 1;
}

const CmdFunc_t lv2_ad9543_cmd_func_list[] = {
		{"spi3_disable_cs", &cli_spi3_disable_cs, "desc: disable spi3 cs\n\tusage: spi3_disable_cs"},
		{"spi3_enable_cs", &cli_spi3_enable_cs, "desc: enable spi3 cs\n\tusage: spi3_enable_cs"},
		{"spi3_transfer", &cli_spi3_transfer, "desc: transfer data through spi3\n\tusage: spi3_transfer data(0x**)"},
		{"ad9543_write", &cli_ad9543_write, "desc: write ad9543 register\n\t"
				"usage: ad9543_write reg_addr(0x****) reg_val(0x**)"},
		{"ad9543_read", &cli_ad9543_read, "desc: read ad9543 register\n\t"
				"usage: ad9543_read reg_addr(0x****)"},
		{"help", NULL, "help cmd to show the manual of the command.\nhelp to show all the commands."},
		{"quit", &lv2_ad9543_quit, "quit ad9543 module"},
		{NULL, NULL, NULL}
};

Cmd_t lv2_ad9543 = {
		"EMetaSpace/ad9543> ",
		"Welcome to ad9543. Type help or ? to list commands",
		lv2_ad9543_cmd_func_list
};
/*********************lv2_ad9543 end*************************/


