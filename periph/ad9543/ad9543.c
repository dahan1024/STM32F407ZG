/*
 * ad9543.c
 *
 *  Created on: 2021Äê1ÔÂ11ÈÕ
 *      Author: cjl
 */


#include <stdio.h>
#include <string.h>

#include <common.h>
#include <drv_spi.h>

#include "ad9543.h"

#define	AD9543_SPI					SPI3
#define AD9543_CS_DISABLE			SPI3_CS1_DISABLE
#define AD9543_CS_ENABLE			SPI3_CS1_ENABLE

void ad9543_init(void)
{
	spi_init(AD9543_SPI);
}

/**************************************************************************//**
* @brief Writes data into a register.
*
* @param dev      - The device structure.
* @param reg_addr - The address of the register to be written.
* @param reg_val  - The value to be written into the register.
*
* @return Returns 0 in case of success or negative error code.
******************************************************************************/
int32_t ad9543_write(uint16_t reg_addr, uint8_t reg_val)
{
	int32_t ret = 0x00;
	uint8_t write_buffer[3];
	int32_t i;

	reg_addr += AD9543_WRITE;

	write_buffer[0] = (uint8_t)((reg_addr & 0xFF00) >> 8);
	write_buffer[1] = (uint8_t)(reg_addr & 0x00FF);
	write_buffer[2] = reg_val;

	AD9543_CS_ENABLE;

	for (i = 0; i < 3; i++)
		spi_transfer(AD9543_SPI, write_buffer[i]);

	AD9543_CS_DISABLE;

	return ret;
}

/**************************************************************************//**
* @brief Reads data from a register.
*
* @param dev      - The device structure.
* @param reg_addr - The address of the register to be read.
* @param reg_val  - The value read from the register.
*
* @return Returns the read data or negative error code.
******************************************************************************/
int32_t ad9543_read(uint16_t reg_addr, uint8_t *reg_val)
{
	int32_t ret = 0x00;
	uint8_t write_buffer[3];
	int32_t i;

	reg_addr += AD9543_READ;

	write_buffer[0] = (uint8_t)((reg_addr & 0xFF00) >> 8);
	write_buffer[1] = (uint8_t)(reg_addr & 0x00FF);
	write_buffer[2] = 0;

	AD9543_CS_ENABLE;

	for (i = 0; i < 2; i++)
		spi_transfer(AD9543_SPI, write_buffer[i]);

	*reg_val = spi_transfer(AD9543_SPI, write_buffer[2]);

	AD9543_CS_DISABLE;

	return ret;
}




