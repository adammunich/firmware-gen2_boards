/*
 * EGG Electric Unicycle firmware
 *
 * Copyright (C) Casainho, 2015, 2106.
 *
 * Released under the GPL License, Version 3
 */

#include "stm32f10x.h"
#include "gpio.h"
#include "stm32f10x_usart.h"

void usart1_bluetooth_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// USART for bluetooth module pins
	GPIO_InitStructure.GPIO_Pin = USART_RX__PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(USART_RX__PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = USART_TX__PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(USART_TX__PORT, &GPIO_InitStructure);

	/* Enable GPIO clock */
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO,
	//					   ENABLE);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,
							ENABLE);

	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl =
		USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	//USART_Init(USART1, &USART_InitStructure);
	USART_Init(USART3, &USART_InitStructure);

	/* Enable the USART1 */
	//USART_Cmd(USART1, ENABLE);
	USART_Cmd(USART3, ENABLE);
}

unsigned char usart1_send_char(unsigned char c)
{
#if 0
	//while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);

	//USART_SendData(USART1, c);
	USART_SendData(USART3, c);

	return (c);
#endif
}

void usart1_send_str(unsigned char *data)
{
#if 0
	unsigned char i = 0, r;

	while((r = data[i++]))
		usart1_send_char(r);
#endif
}

unsigned char usart1_receive_char(void)
{
#if 0
	//while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
	while(USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET);

	//return (USART_ReceiveData(USART1));
	return (USART_ReceiveData(USART3));
#endif
}
