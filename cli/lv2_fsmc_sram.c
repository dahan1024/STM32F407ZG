/*
 * lv2_fsmc_sram.c
 *
 *  Created on: 2021Äê1ÔÂ14ÈÕ
 *      Author: cjl
 */

#include <stdlib.h>

#include <common.h>
#include <types.h>
#include <drv_fsmc_sram.h>

#include "cli_server.h"

/*********************lv2_fsmc_sram begin*************************/
static int cli_write_byte(Cmd_t *cmd_p, int argc, void **argv) {
	uint32_t start_address;
	uint8_t data;
	char *ptr;

	if (argc != (1 + 2))
	{
		error("Argument counter is illegal\n");
		return 0;
	}

	start_address = (uint32_t)strtoul(argv[1], &ptr, 16);
	data = (uint8_t)strtoul(argv[2], &ptr, 16);

	if (fsmc_sram_write_buffer(start_address, &data, 1))
	{
		printf("write_byte(%s): failed\n", (char *)argv[1]);
		return 0;
	}
	else
		printf("write_byte(%s): succeeded\n", (char *)argv[1]);

	return 0;
}

static int cli_read_byte(Cmd_t *cmd_p, int argc, void **argv) {
	uint32_t start_address;
	uint8_t data;
	char *ptr;

	if (argc != 2)
	{
		error("Argument counter is illegal\n");
		return 0;
	}

	start_address = (uint32_t)strtoul(argv[1], &ptr, 16);

	if (fsmc_sram_read_buffer(start_address, &data, 1))
	{
		printf("read_byte(): failed\n");
		return 0;
	}
	else
		printf("read_byte(): succeeded. Return 0x%02x\n", data);

	return 0;
}

static int lv2_fsmc_sram_quit(Cmd_t *cmd_p, int argc, void **argv) {
	set_cur_cmd(&lv1);
	cli_server_cmd_loop(NULL);
	return 1;
}

const CmdFunc_t lv2_fsmc_sram_cmd_func_list[] = {
		{"write_byte", &cli_write_byte, "desc: write a byte to fsmc sram\n"
				"usage: write_byte start_address(0x********) data(0x**)"},
		{"read_byte", &cli_read_byte, "desc: read a byte from fsmc sram\n"
				"usage: read_byte start_address(0x********)"},
		{"help", NULL, "help cmd to show the manual of the command.\nhelp to show all the commands."},
		{"quit", &lv2_fsmc_sram_quit, "quit fsmc_sram module"},
		{NULL, NULL, NULL}
};

Cmd_t lv2_fsmc_sram = {
		"EMetaSpace/fsmc_sram> ",
		"Welcome to fsmc_sram. Type help or ? to list commands",
		lv2_fsmc_sram_cmd_func_list
};
/*********************lv2_fsmc_sram end*************************/





