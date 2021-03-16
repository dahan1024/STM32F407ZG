/*
 * drv_i2c.h
 *
 *  Created on: 2019Äê8ÔÂ16ÈÕ
 *      Author: yanglinjun
 */

#ifndef DRIVERS_DRV_I2C_H_
#define DRIVERS_DRV_I2C_H_

#include <types.h>
#include <stm32f4xx.h>

// I2C state macro definition
#define I2C_READY			0x00
#define I2C_BUS_BUSY		0x01
#define I2C_BUS_ERROR		0x02

// I2C ACK/NACK macro definition
#define I2C_NACK			0x00
#define I2C_ACK				0x01

#define I2C_OK				0
#define I2C_ERR				1


// Functions definition
void i2c_init(void);
u8 I2C_START(void);
void I2C_STOP(void);
u8 I2C_SendByte(u8 i2c_data);
u8 I2C_ReceiveByte(void);
u8 I2C_ReceiveByte_WithACK(void);

uint8_t I2C_Scan(uint8_t slave_address);
uint8_t I2C_Devices(void);

#endif /* DRIVERS_DRV_I2C_H_ */


