/*
 * EGG Electric Unicycle firmware
 *
 * Copyright (C) Casainho, 2015, 2106, 2017.
 *
 * Released under the GPL License, Version 3
 */

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stdio.h"
#include "core_cm3.h"
#include "adc.h"
#include "gpio.h"
#include "pwm.h"
#include "motor.h"
#include "qfplib-m3.h"
#include "math.h"
#include "main.h"
#include "filter.h"
#include "IMU/imu.h"
#include "pwm_duty_cycle_controller.h"

volatile unsigned int motor_speed_erps = 0; // motor speed in electronic rotations per second
volatile unsigned int PWM_cycles_per_SVM_TABLE_step = 0;
volatile unsigned int PWM_cycles_counter = 0;
volatile unsigned int interpolation_PWM_cycles_counter = 0;
volatile int motor_rotor_position = 0; // in degrees
volatile unsigned int motor_rotor_absolute_position = 0; // in degrees
volatile unsigned int interpolation_counter = 0;
volatile int position_correction_value = 0; // in degrees
volatile int interpolation_angle_step = 0; // x1000
volatile int interpolation_sum = 0; // x1000
volatile int interpolation_angle = 0;

volatile int adc_phase_a_current = 0;
volatile int adc_phase_b_current = 0;
volatile int adc_phase_c_current = 0;

volatile int adc_phase_a_current1 = 0;
volatile int adc_phase_c_current1 = 0;
volatile int adc_phase_current1_cycles = 0;

volatile int adc_phase_a_current_offset;
volatile int adc_phase_c_current_offset;

// runs every 1ms
void FOC_slow_loop (void)
{
  //---------------------------
  // Clarke transform assuming balanced currents
  // removing DC offset
//GPIO_SetBits(BUZZER__PORT, BUZZER__PIN);
  __disable_irq();
  adc_phase_a_current = adc_phase_a_current1;
  adc_phase_a_current1 = 0;
  adc_phase_c_current = adc_phase_c_current1;
  adc_phase_c_current1 = 0;
  int temp_adc_phase_current1_cycles = adc_phase_current1_cycles;
  adc_phase_current1_cycles = 0;
  int temp_motor_rotor_position = motor_rotor_position;
  unsigned int temp_direction = get_motor_rotation_direction();
  __enable_irq();

  adc_phase_a_current /= temp_adc_phase_current1_cycles;
  adc_phase_c_current /= temp_adc_phase_current1_cycles;

  adc_phase_a_current = adc_phase_a_current - adc_phase_a_current_offset;
  adc_phase_c_current = adc_phase_c_current - adc_phase_c_current_offset;


  /* Calc phase B current assuming balanced currents
   * considering: a + b + c = 0 ; a + c = -b ; b = -(a + c) ; b = -a -c
   */
#if MOTOR_TYPE == MOTOR_TYPE_EUC1
  adc_phase_b_current = -adc_phase_a_current - adc_phase_c_current;
#elif MOTOR_TYPE == MOTOR_TYPE_EUC2
  // change currents as for this motor, the phases are: BAC and not ABC
  adc_phase_b_current = adc_phase_a_current;
  adc_phase_a_current = -adc_phase_b_current - adc_phase_c_current;
#elif MOTOR_TYPE == MOTOR_TYPE_MICROWORKS_500W_30KMH
  adc_phase_b_current = adc_phase_c_current;
  adc_phase_c_current = adc_phase_a_current;
  adc_phase_a_current = -adc_phase_b_current - adc_phase_c_current;
#endif

  // calc ia and ib in Amps
  float ia = qfp_fmul(adc_phase_a_current, ADC_CURRENT_GAIN_MILLIAMPS);
  float ib = qfp_fmul(adc_phase_b_current, ADC_CURRENT_GAIN_MILLIAMPS);
  float ic = qfp_fmul(adc_phase_c_current, ADC_CURRENT_GAIN_MILLIAMPS);

  float id = 0;
  float iq = 0;


////      int temp_motor_rotor_position1 = temp_motor_rotor_position;
////      temp_motor_rotor_position = (temp_motor_rotor_position + (270 - 1)) % 360; // this makes the motor to run (almost) smooth in both directions -- id current seems good after this
//      float motor_rotor_position_radians = degrees_to_radiands(temp_motor_rotor_position);
//
//      // ABC->dq Park transform
//      //------------------------------------------------------------------------
//      float temp;
//      temp = qfp_fmul(ia, qfp_fcos(motor_rotor_position_radians));
//      temp += qfp_fmul(ib, qfp_fcos(motor_rotor_position_radians + DEGRES_120_IN_RADIANS));
//      temp += qfp_fmul(ic, qfp_fcos(motor_rotor_position_radians - DEGRES_120_IN_RADIANS));
//      id = qfp_fmul(temp, 2.0/3.0);
//
////      temp_motor_rotor_position = (temp_motor_rotor_position1 + (315 - 1)) % 360; // needed for correct value calculation of iq
////      motor_rotor_position_radians = degrees_to_radiands(temp_motor_rotor_position);
//      temp = qfp_fmul(ia, qfp_fsin(motor_rotor_position_radians));
//      temp += qfp_fmul(ib, qfp_fsin((motor_rotor_position_radians) + DEGRES_120_IN_RADIANS));
//      temp += qfp_fmul(ic, qfp_fsin((motor_rotor_position_radians) - DEGRES_120_IN_RADIANS));
//      iq = qfp_fmul(temp, 2.0/3.0);
////      if (temp_direction == RIGHT) { // needed for correct sign value of iq
////	iq *= -1.0;
////      }
//      //------------------------------------------------------------------------

//    m_motor_state.i_alpha = ia;
//    m_motor_state.i_beta = ONE_BY_SQRT3 * ia + TWO_BY_SQRT3 * ib;
//
//    state_m->id = c * state_m->i_alpha + s * state_m->i_beta;
//    state_m->iq = c * state_m->i_beta - s * state_m->i_alpha;


  float i_alpha = ia;
  float i_beta = qfp_fdiv(qfp_fsub(ib, ic), ONE_BY_SQRT3);

  temp_motor_rotor_position = (temp_motor_rotor_position + (270 - 1)) % 360; // this makes the motor to run (almost) smooth in both directions -- id current seems good after this
  float motor_rotor_position_radians = degrees_to_radiands(temp_motor_rotor_position);
  id = qfp_fadd(qfp_fmul(i_alpha, qfp_fcos(motor_rotor_position_radians)), qfp_fmul(i_beta, qfp_fsin(motor_rotor_position_radians)));
  iq = qfp_fsub(qfp_fmul(i_beta, qfp_fcos(motor_rotor_position_radians)), qfp_fmul(i_alpha, qfp_fsin(motor_rotor_position_radians)));

  // Filter Id and Iq currents
  //------------------------------------------------------------------------
  float alpha_idiq = 5.0;
  static float moving_average_id = 0.0;
  static float moving_average_iq = 0.0;
  id = ema_filter_float(&id, &moving_average_id, &alpha_idiq);
  iq = ema_filter_float(&iq, &moving_average_iq, &alpha_idiq);
  // ------------------------------------------------------------------------

  // Calculate phase/angle correction value to try keep id current = 0
  //------------------------------------------------------------------------
  static float correction_value = 0;
  correction_value = qfp_fsub(correction_value, qfp_fmul(K_POSITION_CORRECTION_VALUE, id));

  if ((duty_cycle < 5 && duty_cycle > -5) || motor_speed_erps < 80) // avoid PI controller windup
  { // motor_speed_erps < 80 seems a good value to avoid motor stalling at start up, very low speed
    correction_value = 0.0;
  }
  if (correction_value > 30.0) { correction_value = 30.0; }
  if (correction_value < -30.0) { correction_value = -30.0; }
  position_correction_value = (int) correction_value;
  // ------------------------------------------------------------------------

  static unsigned int loop_timer1 = 0;
  loop_timer1++;
  if (loop_timer1 > 1)
  {
    loop_timer1 = 0;
    balance_controller ();
  }

  static unsigned int loop_timer = 0;
  loop_timer++;
  if (loop_timer > 10)
  {
    loop_timer = 0;

    int motor_speed = (int) motor_speed_erps;
    if (get_motor_rotation_direction() == LEFT) { motor_speed *= -1; }
//    printf ("%.2f, %d, %d\n", angle_error_log, duty_cycle, motor_speed);
//    printf ("%.2f, %d, %d\n", correction_value, duty_cycle, motor_speed);
    printf ("%.2f, %.2f, %.2f, %d, %d\n", id, iq, correction_value, motor_speed, duty_cycle);
//    printf ("%d, %d, %.2f, %.2f\n", motor_speed, duty_cycle, angle_log, angle_error_log);
  }
}

// runs every 50us (PWM frequency)
void FOC_fast_loop (void)
{
  // count number of fast loops / PWM cycles
  if (PWM_cycles_counter < PWM_CYCLES_COUNTER_MAX)
  {
    PWM_cycles_counter++;
  }
  else
  {
    PWM_cycles_counter = 0;
    motor_speed_erps = 0;
    interpolation_angle_step = (SVM_TABLE_LEN * 1000) / PWM_CYCLES_COUNTER_MAX;
    interpolation_sum = 0;
  }

#define DO_INTERPOLATION 1
#if DO_INTERPOLATION == 1
  // calculate the interpolation angle
  // interpolation seems a problem when motor starts, so avoid to do it at very low speed
  if ( !(duty_cycle < 5 && duty_cycle > -5) || motor_speed_erps >= 80)
  {
    if (interpolation_angle_step <= (60 * 1000)) // interpolate only for angle <= 60º
    {
      // add step interpolation value to motor_rotor_position
      interpolation_sum += interpolation_angle_step;
      interpolation_angle = interpolation_sum / 1000;

      if (get_motor_rotation_direction() == RIGHT) {
	motor_rotor_position = (motor_rotor_absolute_position + position_correction_value - interpolation_angle) % 360;
	if (motor_rotor_position < 0) { motor_rotor_position *= -1; } // angle value can be negative in some values of position_correction_value negative, need to convert to positive
      }
      else {
	motor_rotor_position = (motor_rotor_absolute_position + position_correction_value + interpolation_angle) % 360;
	if (motor_rotor_position < 0) { motor_rotor_position *= -1; } // angle value can be negative in some values of position_correction_value negative, need to convert to positive
      }
    }
  }
#endif

  pwm_duty_cycle_controller ();

  // measure raw currents A and C and filter
  adc_phase_a_current1 += (int) adc_get_phase_a_current_value ();
  adc_phase_c_current1 += (int) adc_get_phase_c_current_value ();
  adc_phase_current1_cycles++;
}

// calc the DC offset value for the current ADCs
void motor_calc_current_dc_offset (void)
{
  unsigned int i = 0;
  while (i <= 200)
  {
    i++;
    adc_phase_a_current_offset += adc_get_phase_a_current_value ();
    adc_phase_c_current_offset += adc_get_phase_c_current_value ();
    delay_ms (10);
  }
  adc_phase_a_current_offset /= 200;
  adc_phase_c_current_offset /= 200;
}

void hall_sensors_read_and_action (void)
{
  unsigned int hall_sensors = 0;
  static unsigned int flag_count_speed = 0;

  // read hall sensors signal pins and mask other pins
  hall_sensors = (GPIO_ReadInputData (HALL_SENSORS__PORT) & (HALL_SENSORS_MASK));

  if (get_motor_rotation_direction() == LEFT)
  {
    switch (hall_sensors) // angle increments with rotation
    {
      // measured 12º of advanced phase over hall sensor signal
      case 8192:
      motor_rotor_absolute_position = (60 * 5) + MOTOR_ROTOR_DELTA_PHASE_ANGLE_LEFT; // 6
      break;

      case 24576: // transition to positive value of hall sensor A
      motor_rotor_absolute_position = (60 * 4) + MOTOR_ROTOR_DELTA_PHASE_ANGLE_LEFT; // 5
      break;

      case 16384:
      motor_rotor_absolute_position = (60 * 3) + MOTOR_ROTOR_DELTA_PHASE_ANGLE_LEFT; // 4
      flag_count_speed = 1;
      break;

      case 20480:
      motor_rotor_absolute_position = (60 * 2) + MOTOR_ROTOR_DELTA_PHASE_ANGLE_LEFT; // 3
      break;

      case 4096:
      motor_rotor_absolute_position = (60 * 1) + MOTOR_ROTOR_DELTA_PHASE_ANGLE_LEFT; // 2
      break;

      case 12288:
      motor_rotor_absolute_position = (60 * 0) + MOTOR_ROTOR_DELTA_PHASE_ANGLE_LEFT; // 1

      // count speed only when motor did rotate half of 1 electronic rotation
      if (flag_count_speed)
      {
	  flag_count_speed = 0;
	  motor_speed_erps = PWM_CYCLES_COUNTER_MAX / PWM_cycles_counter;
	  interpolation_angle_step = (SVM_TABLE_LEN * 1000) / PWM_cycles_counter;
	  interpolation_sum = 0;
	  PWM_cycles_counter = 0;
      }
      break;

      default:
      return;
      break;
    }

    motor_rotor_position = (motor_rotor_absolute_position + position_correction_value) % 360;
    if (motor_rotor_position < 0) { motor_rotor_position *= -1; } // angle value can be negative in some values of position_correction_value negative, need to convert to positive
    interpolation_counter = 0;
    interpolation_PWM_cycles_counter = 0;
  }
  else if (get_motor_rotation_direction() == RIGHT)
  {
    switch (hall_sensors) // angle DEcrements with rotation
    {
      // measured 12º of advanced phase over hall sensor signal
      case 8192:
      motor_rotor_absolute_position = (60 * 2) + MOTOR_ROTOR_DELTA_PHASE_ANGLE_RIGHT; // 3
      break;

      case 24576: // transition to positive value of hall sensor A
      motor_rotor_absolute_position = (60 * 1) + MOTOR_ROTOR_DELTA_PHASE_ANGLE_RIGHT; // 2
      break;

      case 16384:
      motor_rotor_absolute_position = (60 * 0) + MOTOR_ROTOR_DELTA_PHASE_ANGLE_RIGHT; // 1

      // count speed only when motor did rotate half of 1 electronic rotation
      if (flag_count_speed)
      {
	  flag_count_speed = 0;
	  motor_speed_erps = PWM_CYCLES_COUNTER_MAX / PWM_cycles_counter;
	  interpolation_angle_step = (SVM_TABLE_LEN * 1000) / PWM_cycles_counter;
	  interpolation_sum = 0;
	  PWM_cycles_counter = 0;
      }
      break;

      case 20480:
      motor_rotor_absolute_position = (60 * 5) + MOTOR_ROTOR_DELTA_PHASE_ANGLE_RIGHT; // 6
      break;

      case 4096:
      motor_rotor_absolute_position = (60 * 4) + MOTOR_ROTOR_DELTA_PHASE_ANGLE_RIGHT; // 5
      break;

      case 12288:
      motor_rotor_absolute_position = (60 * 3) + MOTOR_ROTOR_DELTA_PHASE_ANGLE_RIGHT; // 4
      flag_count_speed = 1;
      break;

      default:
      return;
      break;
    }

    motor_rotor_position = (motor_rotor_absolute_position + position_correction_value) % 360;
    if (motor_rotor_position < 0) { motor_rotor_position *= -1; } // angle value can be negative in some values of position_correction_value negative, need to convert to positive
    interpolation_counter = 0;
    interpolation_PWM_cycles_counter = 0;
  }
}

void hall_sensors_interrupt (void)
{
  hall_sensors_read_and_action ();
}
