/*
 * EGG Electric Unicycle firmware
 *
 * Copyright (C) Casainho, 2015, 2106.
 *
 * Released under the GPL License, Version 3
 */

#include "stm32f10x.h"
#include "gpio.h"

void leds_init(void)
{
	GPIO_PinRemapConfig(GPIO_Remap_PD01, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GLED__PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GLED__PORT, &GPIO_InitStructure);
}

unsigned char led_on(void)
{
	GPIO_SetBits(GLED__PORT,
				 GLED__PIN);
}

unsigned char led_off(void)
{
	GPIO_ResetBits(GLED__PORT,
				   GLED__PIN);
}
