#include <stdio.h>

#include <cli_server.h>
#include <drv_delay.h>
#include <lwip_comm.h>
#include <malloc.h>

#include "system.h"


int main(void)
{
	system_init();

	while (1)
	{
		if (cli_server_rx_state & 0x8000)	// rx completed
		{
			cli_server_rx_buf[cli_server_rx_state & 0x3fff] = '\0';
			cli_server_cmd_loop((char *)cli_server_rx_buf);
			cli_server_rx_state = 0x0000;
		}

	}
}





