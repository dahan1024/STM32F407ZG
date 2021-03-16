/*
 * at24cxx.c
 *
 *  Created on: 2019年8月16日
 *      Author: yanglinjun
 */

#include <drv_i2c.h>
#include <drv_delay.h>

#include "at24cxx.h"

void at24cxx_init(void)
{
	i2c_init();
}

int32_t at24cxx_write_byte(uint16_t register_address, uint8_t data)
{
	if (register_address >= AT24CXX_PAGE_LENGTH * AT24CXX_NUM_PAGES)
		return AT24CXX_ERR;

    if (I2C_START() != I2C_READY)
    {
    	I2C_STOP();
    	return AT24CXX_ERR;
    }

	if (EE_TYPE > AT24C16)
	{
		if (I2C_SendByte(AT24CXX_EEPROM_ADDR) != I2C_ACK)	    //发送写命令
			return AT24CXX_ERR;

		if (I2C_SendByte(register_address >> 8) != I2C_ACK)		//发送高地址
			return AT24CXX_ERR;
	} else
		if (I2C_SendByte(AT24CXX_EEPROM_ADDR + ((register_address / 256) << 1)) != I2C_ACK)
			return AT24CXX_ERR;

    if (I2C_SendByte(register_address % 256) != I2C_ACK)
    	return AT24CXX_ERR;

    if (I2C_SendByte(data) != I2C_ACK)
    	return AT24CXX_ERR;

    I2C_STOP();

    /* memory programming time approximately 5ms */
    delay_ms(5);

    return AT24CXX_OK;
}

int32_t at24cxx_read_byte(uint16_t register_address, uint8_t *data)
{
	if (register_address >= AT24CXX_PAGE_LENGTH * AT24CXX_NUM_PAGES)
		return AT24CXX_ERR;

    if (I2C_START() != I2C_READY)
    {
    	I2C_STOP();
    	return AT24CXX_ERR;
    }

	if (EE_TYPE > AT24C16)
	{
		if (I2C_SendByte(AT24CXX_EEPROM_ADDR) != I2C_ACK)	    //发送写命令
			return AT24CXX_ERR;

		if (I2C_SendByte(register_address >> 8) != I2C_ACK)		//发送高地址
			return AT24CXX_ERR;
	} else
		if (I2C_SendByte(AT24CXX_EEPROM_ADDR + ((register_address / 256) << 1)) != I2C_ACK)
			return AT24CXX_ERR;

    if (I2C_SendByte(register_address % 256) != I2C_ACK)
    	return AT24CXX_ERR;

    if (I2C_START() != I2C_READY)
    {
    	I2C_STOP();
    	return AT24CXX_ERR;
    }

	if (I2C_SendByte(AT24CXX_EEPROM_ADDR + 1) != I2C_ACK)
		return AT24CXX_ERR;

	*data = I2C_ReceiveByte();

	I2C_STOP();

	return AT24CXX_OK;
}

static uint8_t at24cxx_write_page(uint16_t start_address, uint8_t *buf, uint16_t len)
{
	u8 i;

	if ((start_address >= AT24CXX_PAGE_LENGTH * AT24CXX_NUM_PAGES)
			|| !len
			|| (start_address + len > AT24CXX_PAGE_LENGTH * AT24CXX_NUM_PAGES))
		return AT24CXX_ERR;

    if (I2C_START() != I2C_READY)
    {
    	I2C_STOP();
    	return AT24CXX_ERR;
    }

	if (EE_TYPE > AT24C16)
	{
		if (I2C_SendByte(AT24CXX_EEPROM_ADDR) != I2C_ACK)	    //发送写命令
			return AT24CXX_ERR;

		if (I2C_SendByte(start_address >> 8) != I2C_ACK)		//发送高地址
			return AT24CXX_ERR;
	} else
		if (I2C_SendByte(AT24CXX_EEPROM_ADDR + ((start_address / 256) << 1)) != I2C_ACK)
			return AT24CXX_ERR;

    if (I2C_SendByte(start_address % 256) != I2C_ACK)
    	return AT24CXX_ERR;

    for (i = 0; i < len; i++)
        if (I2C_SendByte(buf[i]) != I2C_ACK)
        	return AT24CXX_ERR;

    I2C_STOP();

    /* delay 5 mill seconds */
    delay_ms(5);

    return AT24CXX_OK;
}

int32_t at24cxx_read_buffer(uint16_t start_address, uint8_t *buf, uint16_t len)
{
	if ((start_address >= AT24CXX_PAGE_LENGTH * AT24CXX_NUM_PAGES)
			|| !len
			|| (start_address + len > AT24CXX_PAGE_LENGTH * AT24CXX_NUM_PAGES))
		return AT24CXX_ERR;

    if (I2C_START() != I2C_READY)
    {
    	I2C_STOP();
    	return AT24CXX_ERR;
    }

	if (EE_TYPE > AT24C16)
	{
		if (I2C_SendByte(AT24CXX_EEPROM_ADDR) != I2C_ACK)	    //发送写命令
			return AT24CXX_ERR;

		if (I2C_SendByte(start_address >> 8) != I2C_ACK)		//发送高地址
			return AT24CXX_ERR;
	} else
		if (I2C_SendByte(AT24CXX_EEPROM_ADDR + ((start_address / 256) << 1)) != I2C_ACK)
			return AT24CXX_ERR;

    if (I2C_SendByte(start_address % 256) != I2C_ACK)
    	return AT24CXX_ERR;

    if (I2C_START() != I2C_READY)
    {
    	I2C_STOP();
    	return AT24CXX_ERR;
    }

	if (I2C_SendByte(AT24CXX_EEPROM_ADDR + 1) != I2C_ACK)
		return AT24CXX_ERR;

    while (len)
    {
        if (len == 1)
            *buf = I2C_ReceiveByte();
        else
            *buf = I2C_ReceiveByte_WithACK();
        buf++;
        len--;
    }

	I2C_STOP();

	return AT24CXX_OK;
}

int32_t at24cxx_write_buffer(uint16_t start_address, uint8_t *buf, uint16_t len)
{
	uint16_t index;
	uint16_t page_counter, byte_counter;
	uint16_t offset_address = 0;

	if ((start_address >= AT24CXX_PAGE_LENGTH * AT24CXX_NUM_PAGES)
			|| !len
			|| (start_address + len > AT24CXX_PAGE_LENGTH * AT24CXX_NUM_PAGES))
		return AT24CXX_ERR;

	byte_counter = (AT24CXX_PAGE_LENGTH - (start_address % AT24CXX_PAGE_LENGTH));

	if (len < byte_counter)
	{
		byte_counter = len;
	}

	if (at24cxx_write_page(start_address, buf, byte_counter) == AT24CXX_ERR)
		return AT24CXX_ERR;

	page_counter = ((len - byte_counter) / AT24CXX_PAGE_LENGTH);

	for (index = 0; index < (page_counter); index++)
	{
		offset_address = byte_counter + (index * AT24CXX_PAGE_LENGTH);
		if (at24cxx_write_page((start_address + offset_address), (buf + offset_address), AT24CXX_PAGE_LENGTH) == AT24CXX_ERR)
			return AT24CXX_ERR;
	}

	offset_address = byte_counter + (page_counter * AT24CXX_PAGE_LENGTH);

	if ((len - offset_address) && \
			at24cxx_write_page((start_address + offset_address), (buf + offset_address), (len - offset_address)) == AT24CXX_ERR)
		return AT24CXX_ERR;

	return AT24CXX_OK;
}





