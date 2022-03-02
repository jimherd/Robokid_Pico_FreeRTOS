/**
 * @file Task_drive_motors.c
 * @author Jim Herd
 * @brief   Execute motor move commands
 * @version 0.1
 * @date 2022-02-16
 * 
 */
#include <string.h>

#include "system.h"

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "TMC7300.h"
#include "TMC7300_Registers.h"
#include "TMC7300_Fields.h"

#include "DRV8833_pwm.h"

//==============================================================================
// Main task routine
//==============================================================================
void Task_drive_motors(void *p) {

motor_cmd_packet_t  command;
uint32_t     value;
uint8_t     i;
motor_data_t   temp_motor_data;
uint32_t    start_time, end_time;

    DRV8833_init();

    FOREVER {
        xQueueReceive(queue_motor_cmds, &command,  portMAX_DELAY);
    //
    // get current motor data
    //
        xSemaphoreTake(semaphore_system_IO_data, portMAX_DELAY);
            memcpy(&temp_motor_data, &system_IO_data.push_button_data[command.param1], (sizeof(motor_data_t)));
        xSemaphoreGive(semaphore_system_IO_data);

        switch (command.cmd) {
            case MOTOR_OFF   : 
            case MOTOR_BRAKE : 
            case MOVE        : {
                DRV8833_set_motor(command.param1, command.cmd, command.param2);
                temp_motor_data.motor_state = command.cmd;
                temp_motor_data.pwm_width   = command.param2;
                break;
            }
            default : {
                break;
            }
        }
    //
    // update central system data store
    //
        xSemaphoreTake(semaphore_system_IO_data, portMAX_DELAY);
            memcpy(&system_IO_data.push_button_data[command.param1], &temp_motor_data, (sizeof(motor_data_t)));
        xSemaphoreGive(semaphore_system_IO_data);
    }
}

//        vTaskDelay(3000/portTICK_PERIOD_MS);

//            vTaskDelay(1/portTICK_PERIOD_MS);
//            TMC7300_read_register(IFCNT, &value);
//            execute_cmd(SET_PWM_A, WRITE_CMD, i);
//            vTaskDelay(50/portTICK_PERIOD_MS);


        // for (i=0 ; i<=100 ; i++) {
        //     DRV8833_set_motor(LEFT_MOTOR, MOVE, i);
        //     DRV8833_set_motor(RIGHT_MOTOR, MOVE, i);
        //     vTaskDelay(50/portTICK_PERIOD_MS);
        // }
        // for (i=100 ; i>=0 ; i--) {
        //     DRV8833_set_motor(LEFT_MOTOR, MOVE, i);
        //     DRV8833_set_motor(RIGHT_MOTOR, MOVE, i);
        //     vTaskDelay(50/portTICK_PERIOD_MS);
        // }
        //  for (i=0 ; i>=-100 ; i--) {
        //     DRV8833_set_motor(LEFT_MOTOR, MOVE, i);
        //     DRV8833_set_motor(RIGHT_MOTOR, MOVE, i);
        //     vTaskDelay(50/portTICK_PERIOD_MS);
        // }
        // for (i=-100 ; i<=0 ; i++) {
        //     DRV8833_set_motor(LEFT_MOTOR, MOVE, i);
        //     DRV8833_set_motor(RIGHT_MOTOR, MOVE, i);
        //     vTaskDelay(50/portTICK_PERIOD_MS);
        // }
