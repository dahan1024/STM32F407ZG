#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <common.h>
#include <drv_usart.h>

#include "cli_server.h"

extern Cmd_t lv1;
static Cmd_t *cur_cmd_p = &lv1;

static int cmdhelp(Cmd_t *cmd_p, char *func_name)
{
	const CmdFunc_t *cmd_func_p = cmd_p->func_list;

	if (func_name)
	{
		while (cmd_func_p->name)
		{
			if (!memcmp(cmd_func_p->name, func_name, strlen(cmd_func_p->name)))
			{
				printf("%s\n", cmd_func_p->manual);
				return 0;
			}
			cmd_func_p++;
		}
		printf("%s\n", NOHELP);
		return 0;
	}
	else
	{
		printf("%s\n", DOC_HEADER);

		while (cmd_func_p->name)
		{
			printf("%s ", cmd_func_p->name);
			cmd_func_p++;
		}
		printf("\n");
	}

	return 0;
}

static int onecmd(Cmd_t *cmd_p)
{
	const CmdFunc_t *cmd_func_p = cmd_p->func_list;
	char *args[CLI_MAX_ARGS+1];
	int args_num = 0;
	char *delim = " ";


	args[args_num] = strtok(cmd_p->input_buffer, delim);
	args_num++;

	/*skip blank*/
	if (!args[0])
		return 0;

	//while (args_num < CLI_MAX_ARGS && (args[args_num] = strtok(NULL, " ")))
	//	args_num++;
	while (args_num < CLI_MAX_ARGS)
	{
		args[args_num] = strtok(NULL, delim);
		if (!args[args_num])
			break;

		args_num++;
	}

	args[args_num] = NULL;

	if (!memcmp(args[0], "?", strlen("?")) || !memcmp(args[0], "help", strlen("help")))
	{
		cmdhelp(cmd_p, args[1]);
		return 0;
	}

	while (cmd_func_p->name)
	{
		if ((strlen(args[0]) == strlen(cmd_func_p->name)) && !memcmp(cmd_func_p->name, args[0], strlen(args[0])))
			return cmd_func_p->callback(cmd_p, args_num, (void **)args);
		cmd_func_p++;
	}

	printf("Command %s not found\n", args[0]);
	return 0;
}

static Cmd_t *get_cur_cmd(void) {
	return cur_cmd_p;
}

void set_cur_cmd(Cmd_t *cmd_p) {
	cur_cmd_p = cmd_p;
}

void cli_server_init(void)
{
	usart_init(CLI_UART);
}

int cli_server_cmd_loop(char *input)
{
	Cmd_t *cmd_p;

	cmd_p = get_cur_cmd();

	if (input == NULL)
	{
		return 0;
	}

	if (*input == '\0')
	{
		printf("%s\n", cmd_p->intro);
		printf(cmd_p->prompt);
		//cmdhelp(cmd_p, NULL);
		return 0;
	}

	memset(cmd_p->input_buffer, 0, INPUT_BUFFER_LEN);
	memcpy(cmd_p->input_buffer, input, strlen(input));

	onecmd(cmd_p);

	cmd_p = get_cur_cmd();
	printf("\n%s\n", cmd_p->intro);
	printf(cmd_p->prompt);

	return 0;
}







