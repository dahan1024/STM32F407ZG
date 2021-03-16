/*
 * drv_gpio.c
 *
 *  Created on: 2019Äê8ÔÂ22ÈÕ
 *      Author: yanglinjun
 */

#include "drv_gpio.h"

void gpio_init(GPIO_TypeDef* GPIOx)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    if (GPIOx == GPIOC)
    {
		// Enable GPIOB clock
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
		GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

		GPIO_Init(GPIOC, &GPIO_InitStructure);

		GPIO_ResetBits(GPIOC, GPIO_Pin_5 | GPIO_Pin_6);
    }
}



