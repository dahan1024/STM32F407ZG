/*
 * eeprom.h
 *
 *  Created on: 2020Äê1ÔÂ13ÈÕ
 *      Author: cjl
 */

#ifndef PERIPH_EEPROM_EEPROM_H_
#define PERIPH_EEPROM_EEPROM_H_

#include <types.h>

#include "at24cxx.h"

///////////////////////////////////////////////////////////////////////////////
// EEPROM = Partition0 + Partition1
// Partition0 = selected(1 byte) + EepromConfig_t(EepromConfigNumBytes bytes) + reserved
// Partition1 = selected(1 byte) + EepromConfig_t(EepromConfigNumBytes bytes) + reserved
///////////////////////////////////////////////////////////////////////////////

#define EEPROM_CONFIG_VERSION		1

/*
 * EepromConfig must have a size that is a multiple of 4 bytes.
 * */
typedef struct EepromConfig
{
    ///////////////////////////////////

	uint8_t version;
    uint8_t pac_id;

	uint8_t reserved0[2];

    /** link level hardware address of pc */
    u8_t pc_hwaddr[6];
	uint8_t reserved1[2];

    uint32_t crcAtEnd[1];

} EepromConfig_t;

#define EepromConfigNumBytes	(sizeof(EepromConfig_t)/sizeof(uint8_t))

#define PARTITION_SIZE		(AT24CXX_PAGE_LENGTH * AT24CXX_NUM_PAGES / 2)

enum {Partition0, Partition1};

void eeprom_init(void);

int32_t eeprom_read(EepromConfig_t *eeprom_config_p);

int32_t eeprom_write(EepromConfig_t *eeprom_config_p);

int32_t eeprom_get_pac_id(uint8_t *pac_id_p);

int32_t eeprom_set_pac_id(uint8_t pac_id);

int32_t eeprom_get_pc_hwaddr(uint8_t pc_hwaddr[6]);

int32_t eeprom_set_pc_hwaddr(uint8_t pc_hwaddr[6]);

#endif /* PERIPH_EEPROM_EEPROM_H_ */


