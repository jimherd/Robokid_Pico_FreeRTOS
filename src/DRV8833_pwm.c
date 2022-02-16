/**
 * @file DRV8833_pwm.c
 * @author Jim Herd
 * @brief 
 * @version 0.1
 * @date 2022-02-13
 */
#include    <stdlib.h>

#include    "hardware/pwm.h"

#include    "system.h"
#include    "DRV8833_pwm.h"

vehicle_data_t vehicle_data;
motor_data_t    motor_data[NOS_ROBOKID_MOTORS] = {
    {0, 0, LEFT_MOTOR_FLIP_MODE},      // LEFT MOTOR
    {0, 0, RIGHT_MOTOR_FLIP_MODE},     // RIGHT  MOTOR
};

uint8_t  LM_slice_num, RM_slice_num;

void DRV8833_init(void )
{
    // Left motor PWM control outputs
    gpio_set_function(LEFT_MOTOR_CONTROL_PIN_A, GPIO_FUNC_PWM);
    gpio_set_function(LEFT_MOTOR_CONTROL_PIN_B, GPIO_FUNC_PWM);

    // Right motor PWM control outputs
    gpio_set_function(RIGHT_MOTOR_CONTROL_PIN_A, GPIO_FUNC_PWM);
    gpio_set_function(RIGHT_MOTOR_CONTROL_PIN_B, GPIO_FUNC_PWM);

    // Find out which PWM slice is connected to motoe control pins
    LM_slice_num = pwm_gpio_to_slice_num(LEFT_MOTOR_CONTROL_PIN_A);
    RM_slice_num = pwm_gpio_to_slice_num(RIGHT_MOTOR_CONTROL_PIN_A);

    // This section configures the period of the PWM signals

    pwm_set_clkdiv(LM_slice_num, MOTOR_PWM_CLK_DIV);
    pwm_set_clkdiv(RM_slice_num, MOTOR_PWM_CLK_DIV);

    pwm_set_wrap(LM_slice_num, MOTOR_PWM_FULL_COUNT);
    pwm_set_wrap(RM_slice_num, MOTOR_PWM_FULL_COUNT);
    
    // Ensure both A and B channels have 0% duty cycle.
    pwm_set_chan_level(LM_slice_num, PWM_CHAN_A, 0);
    pwm_set_chan_level(LM_slice_num, PWM_CHAN_B, 0);

    pwm_set_chan_level(RM_slice_num, PWM_CHAN_A, 0);
    pwm_set_chan_level(RM_slice_num, PWM_CHAN_B, 0);

    pwm_set_enabled(LM_slice_num, true);
    pwm_set_enabled(RM_slice_num, true);
}

void set_PWM_duty_cycle(motor_t motor, uint32_t duty_cycle) 
{

}

typedef enum {FORWARD, BACKWARD} direction_t;
typedef enum {NO_FLIP, FLIP} motor_orientation_t;
typedef enum {LOW, HIGH, PWM} DRV8833_in_t;

/**
 * @fn      DRV8833_set_motor(...)
 * @brief   configure a motor
 * 
 * @param motor_number  LEFT_MOTOR or RIGHT_MOTOR
 * @param state         MOTOR_OFF, MOTOR_FORWARD, MOTOR_BACKWARD, or MOTOR_BRAKE
 * @param pwm_width     -100% to +100%
 * @return uint8_t      error code -  OK, BAD_MOTOR_NUMBER, or BAD_PWM_WIDTH
 */
uint8_t  DRV8833_set_motor(motor_t motor_number, motor_cmd_t command, int8_t pwm_width) 
{

uint32_t        pulse_count, period_count ;
motor_cmd_t     left_motor_state, right_motor_state;
uint8_t         pwm_slice;
direction_t     direction;
uint32_t        DRV8833_in1, DRV8833_in2, temp;

    // check parameters

    uint8_t error = OK;
    if (abs(pwm_width) > 100) {
        return FAULT;   // return  BAD_PWM_WIDTH
    }
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
 
    direction = (pwm_width >= 0) ? FORWARD : BACKWARD;
    pulse_count = (abs(pwm_width) * MOTOR_PWM_FULL_COUNT) / 100;
    DRV8833_in1 = LOW;
    DRV8833_in2 = LOW;

    // calculate in1 and in2 motor control signals

    switch (command) {
        case MOTOR_OFF : {       // stop with FREEWHEEL condition
            DRV8833_in1 = LOW;
            DRV8833_in2 = LOW;
            break;
        }
        case MOTOR_BRAKE : {     // stop with BRAKE condition
            DRV8833_in1 = 100;
            DRV8833_in2 = 100;
            break;
        } 
        case MOVE : {
            if (direction == FORWARD) {
                DRV8833_in1 = pulse_count;
                DRV8833_in2 = LOW;
            } else {
                DRV8833_in1 = LOW;
                DRV8833_in2 = pulse_count;
            }
        
        // flip motor control signals if physical motor is opposite orientation
        
            if (motor_data[motor_number].flip == true){
                temp = DRV8833_in2;
                DRV8833_in2 = DRV8833_in1;
                DRV8833_in2 = temp;
            }
            break;
        }
        default : {
            return FAULT;   //  return BAD_MOTOR_COMMAND
        }
    }

    // output in1/in2 motor control signals

    pwm_set_chan_level(pwm_slice, PWM_CHAN_A, DRV8833_in1);
    pwm_set_chan_level(pwm_slice, PWM_CHAN_B, DRV8833_in2);
 
    // log state

    motor_data[motor_number].pwm_width = pwm_width;         // log pulse width
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

    // if ((LM_state == MOTOR_FORWARD) && (RM_state == MOTOR_FORWARD)) { 
    //     vehicle_data.vehicle_state = MOVING_FORWARD;
    //     vehicle_data.speed = 0;
    // } else if ((LM_state == MOTOR_BACKWARD) && (RM_state == MOTOR_BACKWARD)) {
    //     vehicle_data.vehicle_state = MOVING_BACKWARD;
    //     vehicle_data.speed = 0;
    // } else if ((LM_state == MOTOR_BACKWARD) && (RM_state == MOTOR_FORWARD)) {
    //     vehicle_data.vehicle_state = TURNING_LEFT;
    //     vehicle_data.speed = 0;
    // } else if ((LM_state == MOTOR_FORWARD) && (RM_state == MOTOR_BACKWARD)) {
    //     vehicle_data.vehicle_state = TURNING_RIGHT;
    //     vehicle_data.speed = 0;
    // } else {
    //     vehicle_data.vehicle_state = STOPPED;
    //     vehicle_data.speed = 0;
    // }
}
