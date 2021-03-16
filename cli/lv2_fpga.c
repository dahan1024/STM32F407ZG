/*
 * lv2_fpga.c
 *
 *  Created on: 2021Äê1ÔÂ11ÈÕ
 *      Author: cjl
 */

#include <stdio.h>
#include <stdlib.h>

#include <common.h>
#include <fpga.h>
// comment out the following header file in the release version
#include <drv_spi.h>

#include "cli_server.h"

/*********************lv2_fpga begin*************************/
static int cli_fpga_write(Cmd_t *cmd_p, int argc, void **argv) {
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

	if (fpga_write(reg_addr, reg_val) < 0)
		printf("fpga_write(0x%04x, 0x%02x): failed\n", reg_addr, reg_val);
	else
		printf("fpga_write(0x%04x, 0x%02x): succeeded\n", reg_addr, reg_val);

	return 0;
}

static int cli_fpga_read(Cmd_t *cmd_p, int argc, void **argv) {
	uint16_t reg_addr;
	uint8_t reg_val;
	char *ptr;

	if (argc != 2)
	{
		error("argument counter is not 3");

		return 0;
	}

	reg_addr = (uint16_t)strtoul(argv[1], &ptr, 16);

	if (fpga_read(reg_addr, &reg_val) < 0)
		printf("fpga_read(0x%04x): failed\n", reg_addr);
	else
		printf("fpga_read(0x%04x): succeeded. reg_val is 0x%02x\n", reg_addr, reg_val);

	return 0;
}

static int cli_get_pac_id(Cmd_t *cmd_p, int argc, void **argv) {
	uint8_t pac_id;

	if (argc != 1)
	{
		error("Argument counter is illegal\n");
		return 0;
	}

	if (fpga_get_pac_id(&pac_id))
	{
		printf("get_pac_id(): read fpga failed\n");
		return 0;
	}
	else
		printf("get_pac_id(): succeeded. pac_id = 0x%02x\n", pac_id);

	return 0;
}

static int cli_set_pac_id(Cmd_t *cmd_p, int argc, void **argv) {
	uint8_t pac_id;
	char *ptr;

	if (argc != (1 + 1))
	{
		error("Argument counter is illegal\n");
		return 0;
	}

	pac_id = (uint8_t)strtoul(argv[1], &ptr, 16);

	if (fpga_set_pac_id(pac_id))
	{
		printf("set_pac_id(%s): failed\n", (char *)argv[1]);
		return 0;
	}
	else
		printf("set_pac_id(%s): succeeded\n", (char *)argv[1]);

	return 0;
}

static int lv2_fpga_quit(Cmd_t *cmd_p, int argc, void **argv) {
	set_cur_cmd(&lv1);
	cli_server_cmd_loop(NULL);
	return 1;
}

const CmdFunc_t lv2_fpga_cmd_func_list[] = {
		{"fpga_write", &cli_fpga_write, "desc: write fpga register\n\t"
				"usage: fpga_write reg_addr(0x****) reg_val(0x**)"},
		{"fpga_read", &cli_fpga_read, "desc: read fpga register\n\t"
				"usage: fpga_read reg_addr(0x****)"},
		{"get_pac_id", &cli_get_pac_id, "desc: get pac_id\n"
				"usage: get_pac_id"},
		{"set_pac_id", &cli_set_pac_id, "desc: set pac_id\n"
				"usage: set_pac_id pac_id(0x**)"},
		{"help", NULL, "help cmd to show the manual of the command.\nhelp to show all the commands."},
		{"quit", &lv2_fpga_quit, "quit fpga module"},
		{NULL, NULL, NULL}
};

Cmd_t lv2_fpga = {
		"EMetaSpace/fpga> ",
		"Welcome to fpga. Type help or ? to list commands",
		lv2_fpga_cmd_func_list
};
/*********************lv2_fpga end*************************/


