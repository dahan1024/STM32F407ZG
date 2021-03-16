/*
 * lv2_w25qxx.c
 *
 *  Created on: 2021Äê1ÔÂ11ÈÕ
 *      Author: cjl
 */

#include <stdio.h>
#include <stdlib.h>

#include <common.h>
#include <w25qxx.h>
// comment out the following header file in the release version
#include <drv_spi.h>

#include "cli_server.h"

/*********************lv2_w25qxx begin*************************/
static int cli_w25qxx_write(Cmd_t *cmd_p, int argc, void **argv) {
	uint32_t start_address;
	uint8_t value;
	char *ptr;

	if (argc != 3)
	{
		error("argument counter is not 3");

		return 0;
	}

	start_address = (uint32_t)strtoul(argv[1], &ptr, 16);
	value = (uint8_t)strtoul(argv[2], &ptr, 16);

	w25qxx_write(start_address, &value, 1);
	printf("w25qxx_write(0x%08x, 0x%02x): succeeded\n", start_address, value);

	return 0;
}

static int cli_w25qxx_read(Cmd_t *cmd_p, int argc, void **argv) {
	uint32_t start_address;
	uint8_t value;
	char *ptr;

	if (argc != 2)
	{
		error("argument counter is not 3");

		return 0;
	}

	start_address = (uint32_t)strtoul(argv[1], &ptr, 16);

	w25qxx_read(start_address, &value, 1);
	printf("w25qxx_read(0x%08x): succeeded. value is 0x%02x\n", start_address, value);

	return 0;
}

static int lv2_w25qxx_quit(Cmd_t *cmd_p, int argc, void **argv) {
	set_cur_cmd(&lv1);
	cli_server_cmd_loop(NULL);
	return 1;
}

const CmdFunc_t lv2_w25qxx_cmd_func_list[] = {
		{"w25qxx_write", &cli_w25qxx_write, "desc: write w25qxx register\n\t"
				"usage: w25qxx_write start_address(0x********) value(0x**)"},
		{"w25qxx_read", &cli_w25qxx_read, "desc: read w25qxx register\n\t"
				"usage: w25qxx_read start_address(0x********)"},
		{"help", NULL, "help cmd to show the manual of the command.\nhelp to show all the commands."},
		{"quit", &lv2_w25qxx_quit, "quit w25qxx module"},
		{NULL, NULL, NULL}
};

Cmd_t lv2_w25qxx = {
		"EMetaSpace/w25qxx> ",
		"Welcome to w25qxx. Type help or ? to list commands",
		lv2_w25qxx_cmd_func_list
};
/*********************lv2_w25qxx end*************************/


