/*
 * EGG Electric Unicycle firmware
 *
 * Copyright (C) Casainho, 2015, 2106.
 *
 * Released under the GPL License, Version 3
 */

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "gpio.h"

void buzzer_init (void)
{
  //there is nothing to init
  //the buzzer GPIO out pin is already configure on gpio.c
}

void buzzer_on (void)
{
  GPIO_ResetBits(BUZZER__PORT, BUZZER__PIN);
}

void buzzer_off (void)
{
  GPIO_SetBits(BUZZER__PORT, BUZZER__PIN);
}

