#include <cli_server.h>

#include "drv_usart.h"

#define USART1_GPIO_CLOCK		RCC_AHB1Periph_GPIOA
#define USART1_GPIO             GPIOA
#define USART1_TX_PIN          	GPIO_Pin_9
#define USART1_TX_PIN_SOURCE   	GPIO_PinSource9
#define USART1_RX_PIN         	GPIO_Pin_10
#define USART1_RX_PIN_SOURCE  	GPIO_PinSource10

//#define USART2_GPIO_CLOCK		RCC_AHB1Periph_GPIOA
//#define USART2_GPIO             GPIOA
//#define USART2_TX_PIN          	GPIO_Pin_2
//#define USART2_TX_PIN_SOURCE   	GPIO_PinSource2
//#define USART2_RX_PIN         	GPIO_Pin_3
//#define USART2_RX_PIN_SOURCE  	GPIO_PinSource3

#define USART3_GPIO_CLOCK		RCC_AHB1Periph_GPIOB
#define USART3_GPIO             GPIOB
#define USART3_TX_PIN          	GPIO_Pin_10
#define USART3_TX_PIN_SOURCE   	GPIO_PinSource10
#define USART3_RX_PIN         	GPIO_Pin_11
#define USART3_RX_PIN_SOURCE  	GPIO_PinSource11

#define UART4_GPIO_CLOCK		RCC_AHB1Periph_GPIOC
#define UART4_GPIO				GPIOC
#define UART4_TX_PIN          	GPIO_Pin_10
#define UART4_TX_PIN_SOURCE   	GPIO_PinSource10
#define UART4_RX_PIN         	GPIO_Pin_11
#define UART4_RX_PIN_SOURCE  	GPIO_PinSource11

#define UART5_TX_GPIO_CLOCK		RCC_AHB1Periph_GPIOC
#define UART5_RX_GPIO_CLOCK		RCC_AHB1Periph_GPIOD
#define UART5_TX_GPIO			GPIOC
#define UART5_RX_GPIO			GPIOD
#define UART5_TX_PIN          	GPIO_Pin_12
#define UART5_TX_PIN_SOURCE   	GPIO_PinSource12
#define UART5_RX_PIN         	GPIO_Pin_2
#define UART5_RX_PIN_SOURCE  	GPIO_PinSource2

#define BACKSPACE				0x08
#define CR						0x0D		// \r
#define LF						0x0A		// \n
const char backspace_seq[4] = {0x08, 0x20, 0x08, 0x00};
//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 

//__use_no_semihosting was requested, but _ttywrch was referenced
_ttywrch(int ch)
{
	ch = ch;
}

//重定义fputc函数 
int fputc(int ch, FILE *f)
{
	if (ch != '\n')
	{
		while ((USART1->SR & 0X40) == 0);//循环发送,直到发送完毕
		USART1->DR = (u8)ch;
	}
	else
	{
		while ((USART1->SR & 0X40) == 0);//循环发送,直到发送完毕
		USART1->DR = (u8)'\r';

		while ((USART1->SR & 0X40) == 0);//循环发送,直到发送完毕
		USART1->DR = (u8)'\n';
	}
	return ch;
}

#endif

u8 usart1_rx_buf[USART1_BUFFER_SIZE];
u16 usart1_rx_state = 0;

//u8 usart2_rx_buf[USART2_BUFFER_SIZE];
//u16 usart2_rx_state = 0;

//u8 usart3_rx_buf[USART3_BUFFER_SIZE];
//u16 usart3_rx_state = 0;

u8 uart4_rx_buf[UART4_BUFFER_SIZE];
u16 uart4_rx_state = 0;

//初始化IO 串口1 
void usart_init(USART_TypeDef* USARTx) {
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	if (USARTx == USART1)
	{
		RCC_AHB1PeriphClockCmd(USART1_GPIO_CLOCK, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

		GPIO_PinAFConfig(USART1_GPIO, USART1_TX_PIN_SOURCE, GPIO_AF_USART1);
		GPIO_PinAFConfig(USART1_GPIO, USART1_RX_PIN_SOURCE, GPIO_AF_USART1);

		GPIO_InitStructure.GPIO_Pin = USART1_TX_PIN | USART1_RX_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Speed = GPIO_Fast_Speed;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(USART1_GPIO, &GPIO_InitStructure);
	
		USART_InitStructure.USART_BaudRate = 115200u;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		USART_Init(USART1, &USART_InitStructure);

#if USART1_RX_EN
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
#endif

		USART_Cmd(USART1, ENABLE);
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	}
}

void usart_putc(USART_TypeDef* USARTx, char s)
{
	if (s)
	{
		while(!(USARTx->SR & 0x00000040));
		USART_SendData(USARTx, s);
	}
}

void usart_puts(USART_TypeDef* USARTx, char *s)
{
	while(*s)
	{
		while(!(USARTx->SR & 0x00000040));
		USART_SendData(USARTx, *s);
		s++;
	}
}

// Send buffer of bytes
// Input: serial port handler, pointer to first byte of buffer, number of bytes to send
// Returns 0 on success
// Returns -1 on error
int usart_send_buffer(USART_TypeDef* USARTx, unsigned char *tx_array, short bytes_to_send) {
	int i;

	for (i = 0; i < bytes_to_send; i++)
	{
		while(!(USARTx->SR & 0x00000040));
		USART_SendData(USARTx, tx_array[i]);
	}

  	return 0;  // Success
}

#if 1
void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	u8 res;

	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);

		res = USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据

		/* the following lines process a string that ends with 0x0d */
		if ((usart1_rx_state & 0x8000) == 0)//接收未完成
		{
			if (res == BACKSPACE)
			{
				usart_puts(USART1, (char *)backspace_seq);

				if ((usart1_rx_state & 0x00ff) > 0)
					usart1_rx_state--;
			}
			else if (res == CR)
			{
				usart1_rx_buf[usart1_rx_state & 0x00ff] = 0;
				usart_puts(USART1, (char *)usart1_rx_buf);

				usart1_rx_state |= 0x8000;

				/* echo received character */
				usart_putc(USART1, '\r');
				usart_putc(USART1, '\n');
			}
			else if (((res >= '0') && (res <= '9')) || ((res >= 'a') && (res <= 'z'))
					|| ((res >= 'A') && (res <= 'Z')) || (res == '-') || (res == ' ')
					|| (res == '.') || (res == ':') || (res == '?') || (res == '_'))
			{
				usart1_rx_buf[usart1_rx_state & 0x00ff] = res;
				usart1_rx_state++;

				/* echo received character */
				// usart_putc(USART1, res);

				if ((usart1_rx_state & 0x3fff) > (USART1_BUFFER_SIZE - 1))
				{
					usart1_rx_state = 0;//接收数据错误,重新开始接收
					usart_puts(USART1, "Received characters exceed max number.\r\n");
				}
			}
		}
	}
}
#endif




