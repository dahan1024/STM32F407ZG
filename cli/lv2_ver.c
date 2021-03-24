/*
 * lv2_ver.c
 *
 *  Created on: 2020Äê5ÔÂ8ÈÕ
 *      Author: cjl
 */

#include <stdio.h>

#include <common.h>
#include <fpga.h>
#include <netif/ethpac.h>

#include "cli_server.h"

/*********************lv2_ver begin*************************/
static int cli_monitor_ver(Cmd_t *cmd_p, int argc, void **argv) {
	printf("monitor version is %s\n", monitor_ver);

	return 0;
}

static int cli_fpga_ver(Cmd_t *cmd_p, int argc, void **argv) {
	u8_t fpga_ver[9];

	if (ethpac_get_fpga_ver(fpga_ver) < 0)
		printf("ethpac_get_fpga_ver(): failed\n");
	else
	{
		printf("fpga platform version: %.2x%.2x.%.2x%.2x%.2x\n",
				fpga_ver[0], fpga_ver[1], fpga_ver[2], fpga_ver[3], fpga_ver[4]);

		printf("fpga app version: %.2x.%.2x%.2x.%.2x\n",
				fpga_ver[5], fpga_ver[6], fpga_ver[7], fpga_ver[8]);
	}

	return 0;
}

static int lv2_ver_quit(Cmd_t *cmd_p, int argc, void **argv) {
	set_cur_cmd(&lv1);
	cli_server_cmd_loop(NULL);
	return 1;
}

const CmdFunc_t lv2_ver_cmd_func_list[] = {
		{"monitor_ver", &cli_monitor_ver, "desc: get monitor version\n"
				"usage: monitor_ver"},
		{"fpga_ver", &cli_fpga_ver, "desc: get fpga version\n"
				"usage: fpga_ver"},
		{"help", NULL, "help cmd to show the manual of the command.\nhelp to show all the commands."},
		{"quit", &lv2_ver_quit, "quit ver module"},
		{NULL, NULL, NULL}
};

Cmd_t lv2_ver = {
		"EMetaSpace/ver> ",
		"Welcome to ver. Type help or ? to list commands",
		lv2_ver_cmd_func_list
};
/*********************lv2_ver end*************************/





