/*
 * EGG Electric Unicycle firmware
 *
 * Copyright (C) Casainho, 2015, 2106.
 *
 * Released under the GPL License, Version 3
 */

#include "stm32f10x.h"
#include "gpio.h"
#include "adc.h"
#include "pwm.h"
#include "usart.h"
#include "hall_sensors.h"
#include "motor.h"
#include "main.h"
#include "stdio.h"
#include "leds.h"
#include "filter.h"

static unsigned int _ms;

void delay_ms(unsigned int ms)
{
	_ms = 1;
	while(ms >= _ms);
}

void SysTick_Handler(void)		// runs every 1ms
{
	// for delay_ms ()
	_ms++;
}

void initialize(void)
{
	/* Setup SysTick Timer for 1 millisecond interrupts, also enables Systick and Systick-Interrupt */
	if(SysTick_Config(SystemCoreClock / 1000))
	{
		/* Capture error */
		while(1);
	}
//  TIM2_init ();
	gpio_init();
	//adc_init();
	//pwm_init();
//  buzzer_init ();
	//hall_sensor_init();
//  MPU6050_I2C_Init ();
//  MPU6050_Initialize ();

	led_on();
	delay_ms(500);
	led_off();
}

int main(void)
{
	initialize();

	while(1); // FIXME

	int value;

	enable_phase_a();
	enable_phase_b();
	enable_phase_c();

	while(1)
	{

		delay_ms(10);

		value = adc_get_potentiometer_value();
		value = ema_filter(value);

		//value = (value * 1000) / 4096;

		value = value - 2048;
		value = value * 1000;
		value = value / 2048;

		motor_set_duty_cycle(value);
	}
}
