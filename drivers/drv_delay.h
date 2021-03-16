/*
 * drv_delay.h
 *
 *  Created on: 2019Äê8ÔÂ5ÈÕ
 *      Author: yanglinjun
 */

#ifndef DRIVERS_DRV_DELAY_H_
#define DRIVERS_DRV_DELAY_H_

#include <stm32f4xx.h>

void delay_init(void);
void delay_ms(u16 nms);
void delay_us(u32 nus);

#endif /* DRIVERS_DRV_DELAY_H_ */
