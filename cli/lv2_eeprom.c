/*
 * lv2_eeprom.c
 *
 *  Created on: 2021Äê1ÔÂ14ÈÕ
 *      Author: cjl
 */


#include <stdio.h>
#include <stdlib.h>

#include <common.h>
#include <types.h>
#include <eeprom.h>

#include "cli_server.h"

/*********************lv2_eeprom begin*************************/
static int cli_get_pac_id(Cmd_t *cmd_p, int argc, void **argv) {
	uint8_t pac_id;

	if (argc != 1)
	{
		error("Argument counter is illegal\n");
		return 0;
	}

	if (eeprom_get_pac_id(&pac_id))
	{
		printf("get_pac_id(): read eeprom failed\n");
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

	if (eeprom_set_pac_id(pac_id))
	{
		printf("set_pac_id(%s): failed\n", (char *)argv[1]);
		return 0;
	}
	else
		printf("set_pac_id(%s): succeeded\n", (char *)argv[1]);

	return 0;
}

static int cli_get_pc_hwaddr(Cmd_t *cmd_p, int argc, void **argv) {
	uint8_t pc_hwaddr[6];

	if (argc != 1)
	{
		error("Argument counter is illegal\n");
		return 0;
	}

	if (eeprom_get_pc_hwaddr(pc_hwaddr))
	{
		printf("get_pc_hwaddr(): read eeprom failed\n");
		return 0;
	}
	else
		printf("get_pc_hwaddr(): succeeded. pc_hwaddr = %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n",
				pc_hwaddr[0], pc_hwaddr[1], pc_hwaddr[2], pc_hwaddr[3], pc_hwaddr[4], pc_hwaddr[5]);

	return 0;
}

static int cli_set_pc_hwaddr(Cmd_t *cmd_p, int argc, void **argv) {
	uint8_t pc_hwaddr[6];
	char *ptr;
	int i;

	if (argc != (1 + 6))
	{
		error("Argument counter is illegal\n");
		return 0;
	}

	for (i = 0; i < 6; i++)
		pc_hwaddr[i] = (uint8_t)strtoul(argv[1 + i], &ptr, 16);

	if (eeprom_set_pc_hwaddr(pc_hwaddr))
	{
		printf("set_pc_hwaddr(%.2x:%.2x:%.2x:%.2x:%.2x:%.2x): failed\n",
				pc_hwaddr[0], pc_hwaddr[1], pc_hwaddr[2], pc_hwaddr[3], pc_hwaddr[4], pc_hwaddr[5]);
		return 0;
	}
	else
		printf("set_pc_hwaddr(%.2x:%.2x:%.2x:%.2x:%.2x:%.2x): succeeded\n",
				pc_hwaddr[0], pc_hwaddr[1], pc_hwaddr[2], pc_hwaddr[3], pc_hwaddr[4], pc_hwaddr[5]);

	return 0;
}

static int lv2_eeprom_quit(Cmd_t *cmd_p, int argc, void **argv) {
	set_cur_cmd(&lv1);
	cli_server_cmd_loop(NULL);
	return 1;
}

const CmdFunc_t lv2_eeprom_cmd_func_list[] = {
		{"get_pac_id", &cli_get_pac_id, "desc: get pac_id\n"
				"usage: get_pac_id"},
		{"set_pac_id", &cli_set_pac_id, "desc: set pac_id\n"
				"usage: set_pac_id pac_id(0x**)"},
		{"get_pc_hwaddr", &cli_get_pc_hwaddr, "desc: get pc_hwaddr\n"
				"usage: get_pc_hwaddr"},
		{"set_pc_hwaddr", &cli_set_pc_hwaddr, "desc: set pc_hwaddr\n"
				"usage: set_pc_hwaddr pc_hwaddr(0x** 0x** 0x** 0x** 0x** 0x**)"},
		{"help", NULL, "help cmd to show the manual of the command.\nhelp to show all the commands."},
		{"quit", &lv2_eeprom_quit, "quit eeprom module"},
		{NULL, NULL, NULL}
};

Cmd_t lv2_eeprom = {
		"EMetaSpace/eeprom> ",
		"Welcome to eeprom. Type help or ? to list commands",
		lv2_eeprom_cmd_func_list
};
/*********************lv2_eeprom end*************************/




