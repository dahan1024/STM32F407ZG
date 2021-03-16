#ifndef CLI_CLI_COMMON_H_
#define CLI_CLI_COMMON_H_

#include <drv_usart.h>

#define CLI_UART				USART1

#define CLI_TAB 0x09
#define CLI_BACKSPACE 0x7F
#define CLI_NEWLINE '\n'
#define CLI_CAR_RETURN 0x0D

#define CLI_ERR_NOT_FOUND "ERR: command not found"
#define CLI_EXIT_MESSAGE "By By"

#define CLI_MAX_ARGS 16

#define	DOC_LEADER		""
#define	DOC_HEADER		"Documented commands (type help <topic>):"
#define	MISC_HEADER		"Miscellaneous help topics:"
#define	UNDOC_HEADER	"Undocumented commands:"
#define	NOHELP			"*** No help on %s"

struct CmdFunc;

typedef struct Cmd {
    char *prompt;
    char *intro;
	const struct CmdFunc *func_list;		//null terminated list
#define INPUT_BUFFER_LEN	(UART4_BUFFER_SIZE)
    char input_buffer[INPUT_BUFFER_LEN];
} Cmd_t;

typedef struct CmdFunc {
	char *name;
	int (*callback)(Cmd_t *cmd_p, int argc, void **argv);
	char *manual;
} CmdFunc_t;

#define cli_server_rx_buf		usart1_rx_buf
#define cli_server_rx_state		usart1_rx_state

extern Cmd_t lv1;
extern Cmd_t lv2_ad9543;
extern Cmd_t lv2_eeprom;
extern Cmd_t lv2_fpga;
extern Cmd_t lv2_fsmc_sram;
extern Cmd_t lv2_w25qxx;
extern Cmd_t lv2_ver;

void set_cur_cmd(Cmd_t *cmd_p);
void cli_server_init(void);
int cli_server_cmd_loop(char *input);

#endif /* CLI_CLI_COMMON_H_ */

