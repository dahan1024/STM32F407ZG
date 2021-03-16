/**
 * @file    led.h
 * @brief   Light Emitting Diodes control functions.
 * @date    9 kwi 2014
 * @author  Michal Ksiezopolski
 *
 * @details A simple library to add an abstraction
 * layer to blinking LEDs.
 * To use the library you need to call LED_Init and then
 * use LED_Toggle or LED_ChangeState with the initialized
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
 */

#ifndef PERIPHERALS_LED_LED_H_
#define PERIPHERALS_LED_LED_H_

#include <types.h>

/**
 * @defgroup  LED LED
 * @brief     Light Emitting Diode control functions.
 */

/**
 * @addtogroup LED
 * @{
 */

/**
 * @brief LED enum - for identifying an LED.
 */
typedef enum {
  LED0,//!< LED0
  LED1,//!< LED1
  LED2,//!< LED2
  MAX_LEDS
} LED_Number_TypeDef;

/**
 * @brief State of an LED.
 */
typedef enum {
  LED_UNUSED, //!< LED_UNUSED LED not initialized
  LED_OFF,    //!< LED_OFF    Turn off LED
  LED_ON,     //!< LED_ON     Turn on LED
} LED_State_TypeDef;

void led_init(void);
int led_change_state(LED_Number_TypeDef led, LED_State_TypeDef state);
int led_turn_on(LED_Number_TypeDef led);
int led_turn_off(LED_Number_TypeDef led);
int led_toggle(LED_Number_TypeDef led);

#endif /* PERIPHERALS_LED_LED_H_ */



