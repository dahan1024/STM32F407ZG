/*
 * drv_spi.h
 *
 *  Created on: 2019Äê8ÔÂ5ÈÕ
 *      Author: yanglinjun
 */

#ifndef DRIVERS_DRV_SPI_H_
#define DRIVERS_DRV_SPI_H_

#include <types.h>
#include <stm32f4xx.h>

// SPI1_CS
#define SPI1_CS_GPIO			GPIOB
#define SPI1_CS_GPIO_CLOCK		RCC_AHB1Periph_GPIOB
#define SPI1_CS_PIN				GPIO_Pin_14

#define SPI1_CS_DISABLE			GPIO_SetBits(SPI1_CS_GPIO, SPI1_CS_PIN)
#define SPI1_CS_ENABLE			GPIO_ResetBits(SPI1_CS_GPIO, SPI1_CS_PIN)

//// SPI2_CS
//#define SPI2_CS_GPIO			GPIOB
//#define SPI2_CS_GPIO_CLOCK		RCC_AHB1Periph_GPIOB
//#define SPI2_CS_PIN				GPIO_Pin_12
//
//#define SPI2_CS_DISABLE			GPIO_SetBits(SPI2_CS_GPIO, SPI2_CS_PIN)
//#define SPI2_CS_ENABLE			GPIO_ResetBits(SPI2_CS_GPIO, SPI2_CS_PIN)

// SPI3_CS0
#define SPI3_CS0_GPIO			GPIOC
#define SPI3_CS0_GPIO_CLOCK		RCC_AHB1Periph_GPIOC
#define SPI3_CS0_PIN			GPIO_Pin_9

#define SPI3_CS0_DISABLE		GPIO_SetBits(SPI3_CS0_GPIO, SPI3_CS0_PIN)
#define SPI3_CS0_ENABLE			GPIO_ResetBits(SPI3_CS0_GPIO, SPI3_CS0_PIN)

// SPI3_CS1
#define SPI3_CS1_GPIO			GPIOC
#define SPI3_CS1_GPIO_CLOCK		RCC_AHB1Periph_GPIOC
#define SPI3_CS1_PIN			GPIO_Pin_8

#define SPI3_CS1_DISABLE		GPIO_SetBits(SPI3_CS1_GPIO, SPI3_CS1_PIN)
#define SPI3_CS1_ENABLE			GPIO_ResetBits(SPI3_CS1_GPIO, SPI3_CS1_PIN)


///////////////////////////////////////////////////////////////////////////////
// SPI Initialize
///////////////////////////////////////////////////////////////////////////////

void spi_init(SPI_TypeDef *SPIx);

///////////////////////////////////////////////////////////////////////////////
// SPI DeInitialize
///////////////////////////////////////////////////////////////////////////////

void spi_deinit(SPI_TypeDef *SPIx);

///////////////////////////////////////////////////////////////////////////////
// SPI Transfer
///////////////////////////////////////////////////////////////////////////////

uint8_t spi_transfer(SPI_TypeDef *SPIx, uint8_t data);

///////////////////////////////////////////////////////////////////////////////
// Set SPI Divisor
///////////////////////////////////////////////////////////////////////////////

void spi_set_divisor(SPI_TypeDef *SPIx, uint16_t data);

///////////////////////////////////////////////////////////////////////////////

#endif /* DRIVERS_DRV_SPI_H_ */
