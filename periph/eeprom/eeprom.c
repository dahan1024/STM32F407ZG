/*
 * eeprom.c
 *
 *  Created on: 2020Äê1ÔÂ13ÈÕ
 *      Author: cjl
 */

#include <stdio.h>
#include <string.h>

#include <common.h>
#include <drv_crc.h>

#include "eeprom.h"

static uint32_t eeprom_check_crc32b(EepromConfig_t *e, int32_t includeCRCAtEnd);

static int32_t eeprom_check_first_time(BOOL eeprom_reset);

/* True, if we've read the contents of eeprom */
static int32_t eeprom_config_cached = FALSE;

static EepromConfig_t eeprom_config;


static uint32_t eeprom_check_crc32b(EepromConfig_t *e, int32_t includeCRCAtEnd)
{
    return crc32B((uint32_t*)e, includeCRCAtEnd ? (uint32_t*)(e + 1) : e->crcAtEnd);
}

void eeprom_init(void)
{
	at24cxx_init();

	if (eeprom_check_first_time(FALSE))
	{
		error("Failed\n");
		return;
	}
}

static int32_t eeprom_read_cur(EepromConfig_t *eeprom_config_p)
{
	uint16_t start_address;

	start_address = 0x00;

	if (at24cxx_read_buffer(start_address, (uint8_t *)eeprom_config_p, EepromConfigNumBytes))
	{
		error("at24cxx_read_buffer() failed\n");
		return RC_ERROR;
	}

    if (eeprom_check_crc32b(eeprom_config_p, TRUE) != CrcCheckVal)
    {
    	error("eeprom_check_crc32b() failed\n");
    	return RC_ERROR;
    }

    return 0;
}

static int32_t eeprom_read_backup(EepromConfig_t *backup_eeprom_config_p)
{
	uint16_t start_address;

	start_address = 0x00 + PARTITION_SIZE;

	if (at24cxx_read_buffer(start_address, (uint8_t *)backup_eeprom_config_p, EepromConfigNumBytes))
		return RC_ERROR;

    if (eeprom_check_crc32b(backup_eeprom_config_p, TRUE) != CrcCheckVal)
    	return RC_ERROR;

    return 0;
}

static int32_t eeprom_write_necessary(
		uint16_t start_address_base,
		EepromConfig_t *eeprom_config_p,
		EepromConfig_t *eeprom_config_written_p
		)
{
	uint8_t *src1, *src2;
	uint16_t start_address, end_address;

	src1 = (uint8_t *)eeprom_config_written_p;
	src2 = (uint8_t *)eeprom_config_p;

	for (start_address = 0; start_address < EepromConfigNumBytes; start_address++)
	{
		if (src1[start_address] != src2[start_address])
		{
			for (end_address = (start_address + 1); end_address < EepromConfigNumBytes; end_address++)
			{
				if (src1[end_address] == src2[end_address])
				{
					end_address--;
					break;
				}
			}

			if (end_address == EepromConfigNumBytes)
				end_address--;

		    if (at24cxx_write_buffer(start_address_base + start_address, src2 + start_address, (end_address - start_address + 1)))
		    	return RC_ERROR;

			start_address = end_address + 1;
		}
	}

	return 0;
}

static int32_t eeprom_write_cur(EepromConfig_t *eeprom_config_p)
{
    EepromConfig_t eeprom_config_written;
	uint16_t start_address_base;

	start_address_base = 0x00;

	if (at24cxx_read_buffer(start_address_base, (uint8_t *)&eeprom_config_written, EepromConfigNumBytes))
		return RC_ERROR;

	eeprom_config_p->crcAtEnd[0] = crc32B((uint32_t*)eeprom_config_p, eeprom_config_p->crcAtEnd);

    return eeprom_write_necessary(start_address_base, eeprom_config_p, &eeprom_config_written);
}

static int32_t eeprom_write_backup(EepromConfig_t *backup_eeprom_config_p)
{
	EepromConfig_t backup_eeprom_config_written;
	uint16_t start_address_base;

	start_address_base = 0x00 + PARTITION_SIZE;

	if (at24cxx_read_buffer(start_address_base, (uint8_t *)&backup_eeprom_config_written, EepromConfigNumBytes))
		return RC_ERROR;

	backup_eeprom_config_p->crcAtEnd[0] = crc32B((uint32_t*)backup_eeprom_config_p, backup_eeprom_config_p->crcAtEnd);

    return eeprom_write_necessary(start_address_base, backup_eeprom_config_p, &backup_eeprom_config_written);
}

static int32_t eeprom_check_first_time(BOOL eeprom_reset)
{
    uint8_t test_val[2];
	EepromConfig_t cur_eeprom_config, backup_eeprom_config;
	int32_t ret;
	BOOL cur_eeprom_config_bad, backup_eeprom_config_bad;

    if (!eeprom_reset)
    {
		// test_val[0]
		if (at24cxx_read_buffer(0x00, &test_val[0], 1))
			return RC_ERROR;

		// test_val[1]
		if (at24cxx_read_buffer((0x00 + PARTITION_SIZE), &test_val[1], 1))
			return RC_ERROR;

		if (test_val[0] != EEPROM_CONFIG_VERSION || test_val[1] != EEPROM_CONFIG_VERSION)
			eeprom_reset = TRUE;
    }

    if (!eeprom_reset)
    {
    	ret = eeprom_read_cur(&cur_eeprom_config);
    	if (ret)
    		cur_eeprom_config_bad = TRUE;

    	ret = eeprom_read_backup(&backup_eeprom_config);
    	if (ret)
    		backup_eeprom_config_bad = TRUE;

    	if (cur_eeprom_config_bad && backup_eeprom_config_bad)
    		eeprom_reset = TRUE;
    	else if (cur_eeprom_config_bad)
    	{
            ret = eeprom_write_cur(&backup_eeprom_config);
            if (ret)
            	return ret;
    	}
    	else if (backup_eeprom_config_bad)
    	{
            ret = eeprom_write_backup(&cur_eeprom_config);
            if (ret)
            	return ret;
    	}
    }

    if (eeprom_reset)
    {
		// Default settings
        cur_eeprom_config.version = EEPROM_CONFIG_VERSION;

        cur_eeprom_config.pac_id = 0x00;

        memset((void *)&(cur_eeprom_config.reserved0), 0, sizeof(cur_eeprom_config.reserved0));

        ret = eeprom_write_cur(&cur_eeprom_config);
        if (ret)
        	return ret;

        ret = eeprom_write_backup(&cur_eeprom_config);
        if (ret)
        	return ret;
	}

    memcpy((void *)&eeprom_config, (void *)&cur_eeprom_config, EepromConfigNumBytes);

    eeprom_config_cached = TRUE;

    return 0;
}

int32_t eeprom_read(EepromConfig_t *eeprom_config_p)
{
	int32_t ret;

	if (!eeprom_config_cached)
	{
		ret = eeprom_read_cur(&eeprom_config);
		if (ret)
			return ret;

		eeprom_config_cached = TRUE;
	}

	memcpy((void *)eeprom_config_p, (const void *)&eeprom_config, EepromConfigNumBytes);

	return 0;
}

int32_t eeprom_write(EepromConfig_t *eeprom_config_p)
{
	int32_t ret;

	ret = eeprom_write_cur(eeprom_config_p);
	if (ret)
		return ret;

	ret = eeprom_write_backup(eeprom_config_p);
	if (ret)
		return ret;

	memcpy((void *)&eeprom_config, (const void *)eeprom_config_p, EepromConfigNumBytes);

	return 0;
}

// pac_id
int32_t eeprom_get_pac_id(uint8_t *pac_id_p) {
	EepromConfig_t eeprom_config;
	int32_t rc;

	rc = eeprom_read(&eeprom_config);
	if (rc)
	{
		error("Read eeprom failed\n");
		return rc;
	}

	*pac_id_p = eeprom_config.pac_id;

	return 0;
}

int32_t eeprom_set_pac_id(uint8_t pac_id) {
	EepromConfig_t eeprom_config;
	int32_t rc;

	rc = eeprom_read(&eeprom_config);
	if (rc)
	{
		error("read eeprom failed\n");
		return rc;
	}

	eeprom_config.pac_id = pac_id;

	rc = eeprom_write(&eeprom_config);
	if (rc)
	{
		error("Write eeprom failed\n");
		return rc;
	}

	return 0;
}




