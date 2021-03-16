/*
 * w25qxx.h
 *
 *  Created on: 2021年1月15日
 *      Author: cjl
 */

#ifndef PERIPH_FLASH_W25QXX_H_
#define PERIPH_FLASH_W25QXX_H_

#include <types.h>

//W25X系列/Q系列芯片列表
//W25Q80  ID  0XEF13
//W25Q16  ID  0XEF14
//W25Q32  ID  0XEF15
//W25Q64  ID  0XEF16
//W25Q128 ID  0XEF17
#define W25Q80 	0XEF13
#define W25Q16 	0XEF14
#define W25Q32 	0XEF15
#define W25Q64 	0XEF16
#define W25Q128	0XEF17

#define W25QXX_TYPE		W25Q128			//W25QXX芯片型号

#define	W25QXX_CS 		PBout(14)  		//W25QXX的片选信号

//////////////////////////////////////////////////////////////////////////////////
//指令表
#define W25X_WriteEnable		0x06
#define W25X_WriteDisable		0x04
#define W25X_ReadStatusReg		0x05
#define W25X_WriteStatusReg		0x01
#define W25X_ReadData			0x03
#define W25X_FastReadData		0x0B
#define W25X_FastReadDual		0x3B
#define W25X_PageProgram		0x02
#define W25X_BlockErase			0xD8
#define W25X_SectorErase		0x20
#define W25X_ChipErase			0xC7
#define W25X_PowerDown			0xB9
#define W25X_ReleasePowerDown	0xAB
#define W25X_DeviceID			0xAB
#define W25X_ManufactDeviceID	0x90
#define W25X_JedecDeviceID		0x9F

void w25qxx_init(void);
uint16_t w25qxx_read_id(void);  	    	//读取FLASH ID
void w25qxx_read(uint32_t start_address, uint8_t *buf, uint16_t len);	//读取flash
void w25qxx_write(uint32_t start_address, uint8_t *buf, uint16_t len);	//写入flash
void w25qxx_erase_chip(void);    	  	//整片擦除

void w25qxx_power_down(void);        	//进入掉电模式
void w25qxx_wake_up(void);				//唤醒

#endif /* PERIPH_FLASH_W25QXX_H_ */

