/*
 * drv_fsmc_sram.h
 *
 *  Created on: 2021Äê1ÔÂ14ÈÕ
 *      Author: cjl
 */

#ifndef DRIVERS_DRV_FSMC_SRAM_H_
#define DRIVERS_DRV_FSMC_SRAM_H_

#include <types.h>
#include <stm32f4xx.h>

#define FSMC_SRAM_SIZE			0x20000000		// 512MB

void fsmc_sram_init(void);
int32_t fsmc_sram_write_buffer(uint32_t start_address, uint8_t *buf, uint32_t len);
int32_t fsmc_sram_read_buffer(uint32_t start_address, uint8_t *buf, uint32_t len);

#endif /* DRIVERS_DRV_FSMC_SRAM_H_ */

