/**
 * @file    led.c
 * @brief   Light Emitting Diode control functions.
 * @date    9 kwi 2014
 * @author  Michal Ksiezopolski
 *
 * @details A simple library to add an abstraction
 * layer to blinking LEDs.
 * To use the library you need to call led_init and then
 * use led_toggle or led_change_state with the initialized
 * LED number.
 * The various LED ports and pins are defined in
 * led_hal.c and led_hal.h.
 *
 * @verbatim
 * Copyright (c) 2014 Michal Ksiezopolski.
 * All rights reserved. This program and the
 * accompanying materials are made available
 * under the terms of the GNU Public License
 * v3.0 which accompanies this distribution,
 * and is available at
 * http://www.gnu.org/licenses/gpl.html
 * @endverbatim
 *
 */
#include <stm32f4xx.h>

#include "led.h"

#define LED_GPIO		GPIOC

#define LED0_PIN		GPIO_Pin_0
#define LED1_PIN		GPIO_Pin_1
#define LED2_PIN		GPIO_Pin_2

#ifdef DEBUG_LED
	#define print(str, args...) printf("LED--> "str"%s", ##args, "\r")
	#define println(str, args...) printf("LED--> "str"%s", ##args, "\r\n")
#else
	#define print(str, args...) (void)0
	#define println(str, args...) (void)0
#endif

/**
 * @addtogroup LED
 * @{
 */

static LED_State_TypeDef ledState[MAX_LEDS]; ///< States of the LEDs (MAX_LEDS is hardware dependent)

/**
 * @brief Add an LED.
 * @param led LED init structure.
 */
void led_init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	// Enable GPIOB clock
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = LED0_PIN | LED1_PIN | LED2_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

	GPIO_Init(LED_GPIO, &GPIO_InitStructure);

	GPIO_SetBits(LED_GPIO, LED0_PIN | LED1_PIN | LED2_PIN);

	ledState[LED0] = LED_OFF; // LED initially off
	ledState[LED1] = LED_OFF; // LED initially off
	ledState[LED2] = LED_OFF; // LED initially off
}

/**
 * @brief Change the state of an LED.
 * @param led LED number.
 * @param state New state.
 */
int led_change_state(LED_Number_TypeDef led, LED_State_TypeDef state) {
	GPIO_TypeDef *led_gpio;
	uint16_t led_pin;

	if (led >= MAX_LEDS) {
		println("Error: Incorrect LED number %d!", (int)led);
		return -1;
	}

	led_gpio = LED_GPIO;

	switch (led)
	{
	case LED0:
		led_pin = LED0_PIN;
		break;

	case LED1:
		led_pin = LED1_PIN;
		break;

	case LED2:
		led_pin = LED2_PIN;
		break;

	default:
		led_pin = LED0_PIN;
		break;
	}

	if (ledState[led] == LED_UNUSED) {
		println("Error: Uninitialized LED %d!", (int)led);
		return -2;
	} else {
		if (state == LED_OFF) {
			GPIO_SetBits(led_gpio, led_pin); // turn off LED
		} else if (state == LED_ON) {
			GPIO_ResetBits(led_gpio, led_pin); // light up LED
		}
	}

	ledState[led] = state; // update LED state

	return 0;
}

int led_turn_on(LED_Number_TypeDef led) {
	return led_change_state(led, LED_ON);
}

int led_turn_off(LED_Number_TypeDef led) {
	return led_change_state(led, LED_OFF);
}

/**
 * @brief Toggle an LED.
 * @param led LED number.
 */
int led_toggle(LED_Number_TypeDef led) {
	GPIO_TypeDef *led_gpio;
	uint16_t led_pin;

	led_gpio = LED_GPIO;

	switch (led)
	{
	case LED0:
		led_pin = LED0_PIN;
		break;

	case LED1:
		led_pin = LED1_PIN;
		break;

	case LED2:
		led_pin = LED2_PIN;
		break;

	default:
		led_pin = LED0_PIN;
		break;
	}

	if (led >= MAX_LEDS) {
		println("Error: Incorrect LED number %d!", (int)led);
		return -1;
	}

	if (ledState[led] == LED_UNUSED) {
		println("Error: Uninitialized LED %d!", (int)led);
		return -2;
	} else {
		if (ledState[led] == LED_OFF) {
			ledState[led] = LED_ON;
		} else if (ledState[led] == LED_ON) {
			ledState[led]= LED_OFF;
		}

		GPIO_ToggleBits(led_gpio, led_pin);
	}

	return 0;
}


