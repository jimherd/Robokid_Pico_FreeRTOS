
#ifndef __DRV8833_PWM_H__
#define __DRV8833_PWM_H__

#include    "pico/stdlib.h"

#include    "error_codes.h"

#include    "FreeRTOS.h"
#include    "semphr.h"      // from FreeRTOS


void DRV8833_init(void );
void set_PWM_duty_cycle(motor_t motor, uint32_t duty_cycle);
error_codes_e  DRV8833_set_motor(motor_t motor_number, motor_cmd_t cmd, int8_t pwm_width);
void set_vehicle_state(void);

#endif