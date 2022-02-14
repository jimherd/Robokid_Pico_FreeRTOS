/**
 * @file DRV8833_pwm.c
 * @author Jim Herd
 * @brief 
 * @version 0.1
 * @date 2022-02-13
 */
#include    <stdlib.h>

#include    "hardware/pwm.h"
//#include    "hardware/gpio.h"
#include    "system.h"

#include    "DRV8833_pwm.h"

vehicle_data_t vehicle_data;
motor_data_t    motor_data[NOS_ROBOKID_MOTORS];

uint8_t  LM_slice_num, RM_slice_num;

void DRV8833_init(void )
{
    // Left motor PWM control outputs
    gpio_set_function(LEFT_MOTOR_CONTROL_PIN_A, GPIO_FUNC_PWM);
    gpio_set_function(LEFT_MOTOR_CONTROL_PIN_B, GPIO_FUNC_PWM);

    // Right motor PWM control outputs
    gpio_set_function(RIGHT_MOTOR_CONTROL_PIN_A, GPIO_FUNC_PWM);
    gpio_set_function(RIGHT_MOTOR_CONTROL_PIN_B, GPIO_FUNC_PWM);

    // Find out which PWM slice is connected to GPIO 5 (it's slice 2)
    LM_slice_num = pwm_gpio_to_slice_num(LEFT_MOTOR_CONTROL_PIN_A);
    RM_slice_num = pwm_gpio_to_slice_num(RIGHT_MOTOR_CONTROL_PIN_A);

    // This section configures the period of the PWM signals

    pwm_set_clkdiv(LM_slice_num, MOTOR_PWM_CLK_DIV);
    pwm_set_clkdiv(RM_slice_num, MOTOR_PWM_CLK_DIV);

    pwm_set_wrap(LM_slice_num, MOTOR_PWM_FULL_COUNT);
    pwm_set_wrap(LM_slice_num, MOTOR_PWM_FULL_COUNT);
    
    // Ensure both A and B channels have 0% duty cycle.
    pwm_set_chan_level(LM_slice_num, PWM_CHAN_A, 0);
    pwm_set_chan_level(LM_slice_num, PWM_CHAN_B, 0);

    pwm_set_chan_level(RM_slice_num, PWM_CHAN_A, 0);
    pwm_set_chan_level(RM_slice_num, PWM_CHAN_B, 0);
}

void set_PWM_duty_cycle(motor_t motor, uint32_t duty_cycle) 
{

}

//----------------------------------------------------------------------------
// set_motor : configure a motor
// =========
//
// Notes
//      The motor speed is specified in the range of 0 to 100% which is
//      converted to the range 0 to 250 for use in the routine 'SetRatio8"
//
// Parameters
//      unit      : 
//      state     : 
//      pwm_width : 
//

/**
 * @brief 
 * 
 * @param motor_number  LEFT_MOTOR or RIGHT_MOTOR
 * @param state         MOTOR_OFF, MOTOR_FORWARD, MOTOR_BACKWARD, or MOTOR_BRAKE
 * @param pwm_width     0% to 100%
 * @return uint8_t      error code -  OK, BAD_MOTOR_NUMBER, or BAD_PWM_WIDTH
 */
uint8_t  DRV8833_set_motor(motor_t motor_number, motor_cmd_t state, uint8_t pwm_width) 
{

uint32_t        pulse_count, period_count ;
motor_cmd_t     left_motor_state, right_motor_state;
uint32_t        pwm_slice;

    if (pwm_width > 100) {
        return FAULT;   // return  BAD_PWM+WIDTH
    }
    motor_data[motor_number].pwm_width = pwm_width;         // log pulse width
    pulse_count = (pwm_width * MOTOR_PWM_FULL_COUNT) / 100;
    
    uint8_t error = OK;
    switch(motor_number) {
        case LEFT_MOTOR : {
            pwm_slice = LM_slice_num;
            break;
        }
        case RIGHT_MOTOR : {
            pwm_slice = RM_slice_num;
            break;
        }
        default : {
            return FAULT;  // return BAD_MOTOR_NUMBER;
        }  
    }
    
    switch (state) {
        case MOTOR_OFF : {       // stop with FREEWHEEL condition
            pwm_set_chan_level(pwm_slice, PWM_CHAN_A, 0);
            pwm_set_chan_level(pwm_slice, PWM_CHAN_B, 0);
            break;
        }
        case MOTOR_FORWARD : {   // forward with freewheel during OFF time
            pwm_set_chan_level(pwm_slice, PWM_CHAN_A, pwm_width);
            pwm_set_chan_level(pwm_slice, PWM_CHAN_B, 0);
            break;
        }
        case MOTOR_BACKWARD :  { // reverse with freewheel during OFF time
            pwm_set_chan_level(pwm_slice, PWM_CHAN_A, pwm_width);
            pwm_set_chan_level(pwm_slice, PWM_CHAN_B, 0);
            break;
        }
        case MOTOR_BRAKE : {     // stop with BRAKE condition
            pwm_set_chan_level(pwm_slice, PWM_CHAN_A, 100);
            pwm_set_chan_level(pwm_slice, PWM_CHAN_B, 100);
            break;
        } 
        default : {
            return FAULT;   //  return BAD_MOTOR_COMMAND
        }
        motor_data[motor_number].motor_state = state;  // log motor state
    }
    set_vehicle_state(); 
    return OK;
}

//----------------------------------------------------------------------------
// vehicle_stop : set both motor to brake
// ============
//
void vehicle_stop(void) {

    DRV8833_set_motor(LEFT_MOTOR , MOTOR_BRAKE, 0);
    DRV8833_set_motor(RIGHT_MOTOR, MOTOR_BRAKE, 0);
  
    motor_data[LEFT_MOTOR].motor_state = STOPPED;
    motor_data[RIGHT_MOTOR].motor_state = STOPPED;
}

//----------------------------------------------------------------------------
// set_vehicle_state : update state of vehicle
// =================
//
// Notes
//      Uses the state of each motor to define the state of the vehicle
//      -              STOPPED, MOVING_FORWARD, MOVING_BACKWARD, TURNING_LEFT, TURNING_RIGHT
//  7-seg didplay         0           1                 2              3            4
// 
void set_vehicle_state(void) 
{

    uint8_t LM_state = motor_data[LEFT_MOTOR].motor_state;
    uint8_t RM_state = motor_data[RIGHT_MOTOR].motor_state;

    if ((LM_state == MOTOR_FORWARD) && (RM_state == MOTOR_FORWARD)) { 
        vehicle_data.vehicle_state = MOVING_FORWARD;
        vehicle_data.speed = 0;
    } else if ((LM_state == MOTOR_BACKWARD) && (RM_state == MOTOR_BACKWARD)) {
        vehicle_data.vehicle_state = MOVING_BACKWARD;
        vehicle_data.speed = 0;
    } else if ((LM_state == MOTOR_BACKWARD) && (RM_state == MOTOR_FORWARD)) {
        vehicle_data.vehicle_state = TURNING_LEFT;
        vehicle_data.speed = 0;
    } else if ((LM_state == MOTOR_FORWARD) && (RM_state == MOTOR_BACKWARD)) {
        vehicle_data.vehicle_state = TURNING_RIGHT;
        vehicle_data.speed = 0;
    } else {
        vehicle_data.vehicle_state = STOPPED;
        vehicle_data.speed = 0;
    }
}
