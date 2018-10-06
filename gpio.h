/*
 * EGG Electric Unicycle firmware
 *
 * Copyright (C) Casainho, 2015, 2106.
 *
 * Released under the GPL License, Version 3
 */

/* Connections:
 *
 * Motor PHASE_A: yellow wire
 * Motor PHASE_B: blue wire
 * Motor PHASE_C: green wire
 *
 *
 * PIN				        | IN/OUT| Works?|Function
 * ----------------------------------------------------------
 *
 * PA3  (ADC12_IN3)   | in  | ??  | current_phase_A
 * PA2  (ADC12_IN2)   | in  | ??  | current_phase_C
 *
 * PA4  (ADC12_IN4)   | in  | ??  | battery_voltage
 *
 * PB12               | in  | ??  | Hall_sensor_A
 * PB13               | in  | ??  | Hall_sensor_B
 * PB14               | in  | ??  | Hall_sensor_C
 *
 * PB6  (I2C1_SCL)    | in/out| ??  | IMU_MPU6050-SCL
 * PB7  (I2C1_SDA)    | in/out| ??  | IMU_MPU6050-SDA
 *
 * PB1  (TIM3_CH4)    | out | ??  | phase_A-HO_LO
 * PB0  (TIM3_CH3)    | out | ??  | phase_B-HO_LO
 * PA6  (TIM3_CH1)    | out | ??  | phase_C-HO_LO
 * PB2                | out | ??  | phase_A-shutdown
 * PA7                | out | ??  | phase_B-shutdown
 * PA5                | out | ??  | phase_C-shutdown
 *
 * PA9  (USART1_TX)   | out | ??  | usart_tx
 * PA10 (USART1_RX)   | out | ??  | usart_rx
 *
 * PA8                | out | yes | buzzer      (active high: push pull)
 *
 * PB15               | out | ??  | LED_1-battery_indicator (active low: float to disable and GND to turn on)
 * PA11               | out | ??  | LED_2-battery_indicator (active low: float to disable and GND to turn on)
 * PA12               | out | ??  | LED_3-battery_indicator (active low: float to disable and GND to turn on)
 * PB5                | out | ??  | LED_4-battery_indicator (active low: float to disable and GND to turn on)
 *
 */


/* Ninebot Connections
 *
 * PIN				        | IN/OUT| Works?|Function
 * ----------------------------------------------------------
 *
 * PA3  (ADC12_IN3)   | in  | ??  | current_phase_A
 * PA4  (ADC12_IN4)   | in  | ??  | current_phase_C
 * PA5  (ADC12_IN5)   | in  | ??  | current_phase_B
 *
 * PB4  (TIM3_CH3)    | in  | ??  | Hall_sensor_A
 * PB5                | in  | ??  | Hall_sensor_B
 * PB0                | in  | ??  | Hall_sensor_C
 *
 * PA8  (TIM1_CH1)    | out | ??  | phase_A-HI
 * PB13 (TIM1_CH1N)   | out | ??  | phase_A-LO
 * PA9  (TIM1_CH2)    | out | ??  | phase_B-HI
 * PB14 (TIM1_CH2N)   | out | ??  | phase_B-LO
 * PA10 (TIM1_CH3)    | out | ??  | phase_C-HI
 * PA15 (TIM1_CH3N)   | out | ??  | phase_C-LO
 *
 * PB6  (USART1_TX)   | ??  | ??  | "T on 4 pin header"
 *
 * PB10 (USART3_TX)   | out | ??  | "T on 3 pin header"
 * PB11 (USART3_RX)   | in  | ??  | "R on 3 pin header"
 * PA15 			  | out | ??  | Brake light transistor
 *
 * PD1                | out | ??  | LED_1
 *
 */

#ifndef GPIO_H
#define GPIO_H


#if 0
#define LED_1_BATTERY_INDICATOR__PIN        GPIO_Pin_15
#define LED_1_BATTERY_INDICATOR__PORT       GPIOB
#endif

#define CURRENT_PHASE_A__PIN      GPIO_Pin_3
#define CURRENT_PHASE_A__PORT     GPIOA
#define CURRENT_PHASE_C__PIN      GPIO_Pin_2
#define CURRENT_PHASE_C__PORT     GPIOA

#if 0
#define BATTERY_VOLTAGE__PIN      GPIO_Pin_4
#define BATTERY_VOLTAGE__PORT     GPIOA
#endif

#define HALL_SENSOR_A__PIN        GPIO_Pin_4
#define HALL_SENSOR_A__PORT       GPIOB
#define HALL_SENSOR_B__PIN        GPIO_Pin_5
#define HALL_SENSOR_B__PORT       GPIOB
#define HALL_SENSOR_C__PIN        GPIO_Pin_0
#define HALL_SENSOR_C__PORT       GPIOB
#define HALL_SENSORS__PORT        GPIOB

#if 0
#define MPU6050_SCL__PIN          GPIO_Pin_6
#define MPU6050_SCL__PORT         GPIOB
#define MPU6050_SDA__PIN          GPIO_Pin_7
#define MPU6050_SDA__PORT         GPIOB
#endif

#define PHASE_A_HI__PIN           GPIO_Pin_8
#define PHASE_A_HI__PORT          GPIOA
#define PHASE_A_LO__PIN           GPIO_Pin_13
#define PHASE_A_LO__PORT          GPIOB
#define PHASE_B_HI__PIN           GPIO_Pin_9
#define PHASE_B_HI__PORT          GPIOA
#define PHASE_B_LO__PIN           GPIO_Pin_14
#define PHASE_B_LO__PORT          GPIOB
#define PHASE_C_HI__PIN           GPIO_Pin_10
#define PHASE_C_HI__PORT          GPIOA
#define PHASE_C_LO__PIN           GPIO_Pin_15
#define PHASE_C_LO__PORT          GPIOB

#define USART_TX__PIN             GPIO_Pin_10
#define USART_TX__PORT            GPIOB
#define USART_RX__PIN             GPIO_Pin_11
#define USART_RX__PORT            GPIOB

#if 0
#define BUZZER__PIN               GPIO_Pin_8
#define BUZZER__PORT              GPIOA
#endif

#define GLED__PIN               GPIO_Pin_1
#define GLED__PORT              GPIOD

void gpio_init(void);

#endif							/* GPIO_H_ */
