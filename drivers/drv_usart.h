/*
 * drv_usart.h
 *
 *  Created on: 2019年8月5日
 *      Author: yanglinjun
 */

#ifndef DRIVERS_DRV_USART_H_
#define DRIVERS_DRV_USART_H_

#include <stdio.h>

#include <stm32f4xx.h>

#define USART1_RX_EN 			1		//使能（1）/禁止（0）串口1接收
#define USART2_RX_EN 			1		//使能（1）/禁止（0）串口2接收
#define USART3_RX_EN 			1		//使能（1）/禁止（0）串口3接收
#define UART4_RX_EN 			1		//使能（1）/禁止（0）串口4接收
#define UART5_RX_EN 			1		//使能（1）/禁止（0）串口5接收

#define USART1_BUFFER_SIZE  	256  	//定义最大接收字节数 256
#define USART2_BUFFER_SIZE  	256  	//定义最大接收字节数 256
#define USART3_BUFFER_SIZE  	256  	//定义最大接收字节数 256
#define UART4_BUFFER_SIZE  		256  	//定义最大接收字节数 256

extern u8 usart1_rx_buf[USART1_BUFFER_SIZE];
extern u16 usart1_rx_state;

//extern u8  usart2_rx_buf[USART2_BUFFER_SIZE]; //接收缓冲,最大USART2_BUFFER_SIZE个字节.末字节为换行符
//extern u16 usart2_rx_state;         		//接收状态标记

//extern u8  usart3_rx_buf[USART3_BUFFER_SIZE]; //接收缓冲,最大USART3_BUFFER_SIZE个字节.末字节为换行符
//extern u16 usart3_rx_state;         		//接收状态标记

extern u8  uart4_rx_buf[UART4_BUFFER_SIZE]; //接收缓冲,最大UART4_BUFFER_SIZE个字节.末字节为换行符
extern u16 uart4_rx_state;         		//接收状态标记

void usart_init(USART_TypeDef* USARTx);
void usart_puts(USART_TypeDef* USARTx, char *s);
int usart_send_buffer(USART_TypeDef* USARTx, unsigned char *tx_array, short bytes_to_send);

#endif /* DRIVERS_DRV_USART_H_ */
