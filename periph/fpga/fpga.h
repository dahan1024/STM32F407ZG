/*
 * fpga.h
 *
 *  Created on: 2021Äê1ÔÂ19ÈÕ
 *      Author: cjl
 */

#ifndef PERIPH_FPGA_FPGA_H_
#define PERIPH_FPGA_FPGA_H_

#include <types.h>

/******************************************************************************/
/*********************************** FPGA ***********************************/
/******************************************************************************/

extern const char monitor_ver[];

void fpga_init(void);

int32_t fpga_write(uint16_t reg_addr, uint8_t reg_val);

int32_t fpga_read(uint16_t reg_addr, uint8_t *reg_val);

int32_t fpga_set_pac_id(uint8_t pac_id);

int32_t fpga_get_pac_id(uint8_t *pac_id_p);

int32_t fpga_get_mac(uint8_t mac[6]);

int32_t fpga_set_monitor_ver(void);

#endif /* PERIPH_FPGA_FPGA_H_ */


