/*
 * fpga.c
 *
 *  Created on: 2021Äê1ÔÂ19ÈÕ
 *      Author: cjl
 */

#include <stdio.h>
#include <string.h>

#include <common.h>
#include <drv_spi.h>

#include "fpga.h"

#define	FPGA_SPI						SPI3
#define FPGA_CS_DISABLE					SPI3_CS0_DISABLE
#define FPGA_CS_ENABLE					SPI3_CS0_ENABLE

#define FPGA_READ						0x01
#define FPGA_WRITE						0x02

#define FPGA_REG_ADDR_PAC_ID			0x0002

#define FPGA_REG_ADDR_MAC_47_40			0x8100
#define FPGA_REG_ADDR_MAC_39_32			0x8101
#define FPGA_REG_ADDR_MAC_31_24			0x8102
#define FPGA_REG_ADDR_MAC_23_16			0x8103
#define FPGA_REG_ADDR_MAC_15_08			0x8104
#define FPGA_REG_ADDR_MAC_07_00			0x8105

#define FPGA_REG_ADDR_MONITOR_VER_0		0x9010
#define FPGA_REG_ADDR_MONITOR_VER_7		0x9017

const char monitor_ver[] = "V01.12";

void fpga_init(void)
{
	spi_init(FPGA_SPI);
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
int32_t fpga_write(uint16_t reg_addr, uint8_t reg_val)
{
	int32_t ret = 0x00;
	uint8_t write_buffer[4];
	int32_t i;

	write_buffer[0] = FPGA_WRITE;
	write_buffer[1] = (uint8_t)((reg_addr & 0xFF00) >> 8);
	write_buffer[2] = (uint8_t)(reg_addr & 0x00FF);
	write_buffer[3] = reg_val;

	FPGA_CS_ENABLE;

	for (i = 0; i < 4; i++)
		spi_transfer(FPGA_SPI, write_buffer[i]);

	FPGA_CS_DISABLE;

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
int32_t fpga_read(uint16_t reg_addr, uint8_t *reg_val)
{
	int32_t ret = 0x00;
	uint8_t write_buffer[4];
	int32_t i;

	write_buffer[0] = FPGA_READ;
	write_buffer[1] = (uint8_t)((reg_addr & 0xFF00) >> 8);
	write_buffer[2] = (uint8_t)(reg_addr & 0x00FF);
	write_buffer[3] = 0;

	FPGA_CS_ENABLE;

	for (i = 0; i < 3; i++)
		spi_transfer(FPGA_SPI, write_buffer[i]);

	*reg_val = spi_transfer(FPGA_SPI, write_buffer[3]);

	FPGA_CS_DISABLE;

	return ret;
}

int32_t fpga_set_pac_id(uint8_t pac_id) {
	int32_t rc;

	rc = fpga_write(FPGA_REG_ADDR_PAC_ID, pac_id);
	if (rc)
	{
		error("Write fpga failed\n");
		return rc;
	}

	return 0;
}

// pac_id
int32_t fpga_get_pac_id(uint8_t *pac_id_p) {
	int32_t rc;

	rc = fpga_read(FPGA_REG_ADDR_PAC_ID, pac_id_p);
	if (rc)
	{
		error("Read fpga failed\n");
		return rc;
	}

	return 0;
}

// fpga mac
int32_t fpga_get_mac(uint8_t mac[6]) {
	uint16_t reg_addr_list[6];
	int32_t rc;
	int32_t i;

	reg_addr_list[0] = FPGA_REG_ADDR_MAC_47_40;
	reg_addr_list[1] = FPGA_REG_ADDR_MAC_39_32;
	reg_addr_list[2] = FPGA_REG_ADDR_MAC_31_24;
	reg_addr_list[3] = FPGA_REG_ADDR_MAC_23_16;
	reg_addr_list[4] = FPGA_REG_ADDR_MAC_15_08;
	reg_addr_list[5] = FPGA_REG_ADDR_MAC_07_00;

	for (i = 0; i < 6; i++)
	{
		rc = fpga_read(reg_addr_list[i], &mac[i]);
		if (rc)
		{
			error("Read fpga failed\n");
			return rc;
		}
	}

	return 0;
}

int32_t fpga_set_monitor_ver(void) {
	uint8_t buf[8];
	int32_t i;
	int32_t rc;

	memset((void *)buf, 0, 8);
	memcpy((void *)buf, (const void *)monitor_ver, strlen(monitor_ver));

	for (i = 0; i < 8; i++)
	{
		rc = fpga_write(FPGA_REG_ADDR_MONITOR_VER_0 + i, buf[i]);
		if (rc)
		{
			error("Write fpga failed\n");
			return rc;
		}
	}

	return 0;
}




