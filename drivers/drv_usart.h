/*
 * drv_usart.h
 *
 *  Created on: 2019��8��5��
 *      Author: yanglinjun
 */

#ifndef DRIVERS_DRV_USART_H_
#define DRIVERS_DRV_USART_H_

#include <stdio.h>

#include <stm32f4xx.h>

#define USART1_RX_EN 			1		//ʹ�ܣ�1��/��ֹ��0������1����
#define USART2_RX_EN 			1		//ʹ�ܣ�1��/��ֹ��0������2����
#define USART3_RX_EN 			1		//ʹ�ܣ�1��/��ֹ��0������3����
#define UART4_RX_EN 			1		//ʹ�ܣ�1��/��ֹ��0������4����
#define UART5_RX_EN 			1		//ʹ�ܣ�1��/��ֹ��0������5����

#define USART1_BUFFER_SIZE  	256  	//�����������ֽ��� 256
#define USART2_BUFFER_SIZE  	256  	//�����������ֽ��� 256
#define USART3_BUFFER_SIZE  	256  	//�����������ֽ��� 256
#define UART4_BUFFER_SIZE  		256  	//�����������ֽ��� 256

extern u8 usart1_rx_buf[USART1_BUFFER_SIZE];
extern u16 usart1_rx_state;

//extern u8  usart2_rx_buf[USART2_BUFFER_SIZE]; //���ջ���,���USART2_BUFFER_SIZE���ֽ�.ĩ�ֽ�Ϊ���з�
//extern u16 usart2_rx_state;         		//����״̬���

//extern u8  usart3_rx_buf[USART3_BUFFER_SIZE]; //���ջ���,���USART3_BUFFER_SIZE���ֽ�.ĩ�ֽ�Ϊ���з�
//extern u16 usart3_rx_state;         		//����״̬���

extern u8  uart4_rx_buf[UART4_BUFFER_SIZE]; //���ջ���,���UART4_BUFFER_SIZE���ֽ�.ĩ�ֽ�Ϊ���з�
extern u16 uart4_rx_state;         		//����״̬���

void usart_init(USART_TypeDef* USARTx);
void usart_puts(USART_TypeDef* USARTx, char *s);
int usart_send_buffer(USART_TypeDef* USARTx, unsigned char *tx_array, short bytes_to_send);

#endif /* DRIVERS_DRV_USART_H_ */
