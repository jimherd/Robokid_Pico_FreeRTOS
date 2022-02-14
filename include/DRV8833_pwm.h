#include    "pico/stdlib.h"

#include    "FreeRTOS.h"
#include    "semphr.h"      // from FreeRTOS

#ifndef _DRV8833_PWM_H_
#define _DRV8833_PWM_H_



#define     PWM_RAW_FREQ        125000000   // 125MHz
#define     MOTOR_COUNT_FREQ    25000000    // 25MHz
#define     MOTOR_PWM_FREQ      5000        // 5KHz

#define     MOTOR_PWM_CLK_DIV       (125000000/25000000)
#define     MOTOR_PWM_FULL_COUNT    (MOTOR_COUNT_FREQ/MOTOR_PWM_FREQ)

typedef struct {
    uint8_t     motor_state;
    uint8_t     pwm_width;
} motor_data_t;

typedef struct {
    vehicle_state_t     vehicle_state;
    uint32_t            speed;
} vehicle_data_t;

void DRV8833_init(void );
void set_PWM_duty_cycle(motor_t motor, uint32_t duty_cycle);
uint8_t  DRV8833_set_motor(motor_t motor_number, motor_cmd_t state, uint8_t pwm_width);
void set_vehicle_state(void);

#endif