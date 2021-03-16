/*
 * at24cxx.h
 *
 *  Created on: 2019Äê12ÔÂ30ÈÕ
 *      Author: cjl
 */

#ifndef PERIPH_EEPROM_AT24CXX_H_
#define PERIPH_EEPROM_AT24CXX_H_

#include <types.h>

/*
 * Memory Organization
 * AT24C01A, 1K SERIAL EEPROM: Internally organized with 16 pages of 8 bytes each,
 * the 1K requires a 7-bit data word address for random word addressing.
 *
 * AT24C02, 2K SERIAL EEPROM: Internally organized with 32 pages of 8 bytes each,
 * the 2K requires an 8-bit data word address for random word addressing.
 *
 * AT24C04, 4K SERIAL EEPROM: Internally organized with 32 pages of 16 bytes each,
 * the 4K requires a 9-bit data word address for random word addressing.
 *
 * AT24C08A, 8K SERIAL EEPROM: Internally organized with 64 pages of 16 bytes
 * each, the 8K requires a 10-bit data word address for random word addressing.
 *
 * AT24C16A, 16K SERIAL EEPROM: Internally organized with 128 pages of 16 bytes
 * each, the 16K requires an 11-bit data word address for random word addressing.
 * */
#define AT24C01		127
#define AT24C02		255
#define AT24C04		511
#define AT24C08		1023
#define AT24C16		2047
#define AT24C32		4095
#define AT24C64	    8191
#define AT24C128	16383
#define AT24C256	32767

#define AT24CXX_OK		0
#define AT24CXX_ERR		1

/******************at24cxx config******************************/
#define EE_TYPE 	AT24C04

#define AT24CXX_EEPROM_ADDR				0xA0

#define AT24CXX_REGISTER_ADDRESS_WIDTH	16 // 8 or 16
#define AT24CXX_PAGE_LENGTH				16 // 8 , 16 , 32 or 64
#define AT24CXX_NUM_PAGES				32

/******************at24cxx config******************************/


void at24cxx_init(void);

int32_t at24cxx_write_byte(uint16_t register_address, uint8_t data);
int32_t at24cxx_read_byte(uint16_t register_address, uint8_t *data);

int32_t at24cxx_read_buffer(uint16_t start_address, uint8_t *buf, uint16_t len);
int32_t at24cxx_write_buffer(uint16_t start_address, uint8_t *buf, uint16_t len);


#endif /* PERIPH_EEPROM_AT24CXX_H_ */

