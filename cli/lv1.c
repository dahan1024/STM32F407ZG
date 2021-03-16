/*
 * lv1.c
 *
 *  Created on: 2020Äê5ÔÂ8ÈÕ
 *      Author: cjl
 */

#include <common.h>

#include "cli_server.h"

/********************lv1 begin**************************/
static int cli_ad9543(Cmd_t *cmd_p, int argc, void **argv) {
	set_cur_cmd(&lv2_ad9543);
	cli_server_cmd_loop(NULL);
	return 0;
}

static int cli_eeprom(Cmd_t *cmd_p, int argc, void **argv) {
	set_cur_cmd(&lv2_eeprom);
	cli_server_cmd_loop(NULL);
	return 0;
}

static int cli_fpga(Cmd_t *cmd_p, int argc, void **argv) {
	set_cur_cmd(&lv2_fpga);
	cli_server_cmd_loop(NULL);
	return 0;
}

static int cli_fsmc_sram(Cmd_t *cmd_p, int argc, void **argv) {
	set_cur_cmd(&lv2_fsmc_sram);
	cli_server_cmd_loop(NULL);
	return 0;
}

static int cli_w25qxx(Cmd_t *cmd_p, int argc, void **argv) {
	set_cur_cmd(&lv2_w25qxx);
	cli_server_cmd_loop(NULL);
	return 0;
}

static int cli_ver(Cmd_t *cmd_p, int argc, void **argv) {
	set_cur_cmd(&lv2_ver);
	cli_server_cmd_loop(NULL);
	return 0;
}

static int cli_quit(Cmd_t *cmd_p, int argc, void **argv) {
	return 1;
}

const CmdFunc_t lv1_cmd_func_list[] = {
		{"ad9543", &cli_ad9543, "desc: enter ad9543 module"
				"usage: ad9543"},
		{"eeprom", &cli_eeprom, "desc: enter eeprom module\n"
				"usage: eeprom"},
		{"fpga", &cli_fpga, "desc: enter fpga module\n"
				"usage: fpga"},
		{"fsmc_sram", &cli_fsmc_sram, "desc: enter fsmc_sram module\n"
				"usage: fsmc_sram"},
		{"w25qxx", &cli_w25qxx, "desc: enter w25qxx module\n"
				"usage: w25qxx"},
		{"ver", &cli_ver, "desc: enter version module\n"
				"usage: ver"},
		{"help", NULL, "help cmd to show the manual of the command.\nhelp to show all the commands."},
		{"quit", &cli_quit, "quit system"},
		{NULL, NULL, NULL}
};

Cmd_t lv1 = {
		"EMetaSpace> ",
		"Welcome to level 1. Type help or ? to list commands",
		lv1_cmd_func_list
};

/*********************lv1 end*************************/




