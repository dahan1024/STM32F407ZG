/*
 * lv2_ver.c
 *
 *  Created on: 2020Äê5ÔÂ8ÈÕ
 *      Author: cjl
 */

#include <stdio.h>

#include <common.h>
#include <fpga.h>

#include "cli_server.h"

/*********************lv2_ver begin*************************/
static int cli_monitor_ver(Cmd_t *cmd_p, int argc, void **argv) {
	printf("monitor version is %s\n", monitor_ver);

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





