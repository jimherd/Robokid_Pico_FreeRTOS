/**
 * @file Task_drive_motors.c
 * @author Jim Herd
 * @brief   Execute motor move commands
 * @version 0.1
 * @date 2022-02-16
 * 
 */
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

    DRV8833_init();

    FOREVER {
        xQueueReceive(queue_motor_cmds, &command,  portMAX_DELAY);

        switch (command.cmd) {
            case MOTOR_OFF : {
                break;
            }
            case MOTOR_BRAKE : {
                break;
            }
            case MOVE : {
                break;
            }
        }




        for (i=0 ; i<=100 ; i++) {
            DRV8833_set_motor(LEFT_MOTOR, MOVE, i);
            DRV8833_set_motor(RIGHT_MOTOR, MOVE, i);
            vTaskDelay(50/portTICK_PERIOD_MS);
        }
        for (i=100 ; i>=0 ; i--) {
            DRV8833_set_motor(LEFT_MOTOR, MOVE, i);
            DRV8833_set_motor(RIGHT_MOTOR, MOVE, i);
            vTaskDelay(50/portTICK_PERIOD_MS);
        }
         for (i=0 ; i>=-100 ; i--) {
            DRV8833_set_motor(LEFT_MOTOR, MOVE, i);
            DRV8833_set_motor(RIGHT_MOTOR, MOVE, i);
            vTaskDelay(50/portTICK_PERIOD_MS);
        }
        for (i=-100 ; i<=0 ; i++) {
            DRV8833_set_motor(LEFT_MOTOR, MOVE, i);
            DRV8833_set_motor(RIGHT_MOTOR, MOVE, i);
            vTaskDelay(50/portTICK_PERIOD_MS);
        }
        //
        // update central system data store
        //

        vTaskDelay(3000/portTICK_PERIOD_MS);
    }
}

//            vTaskDelay(1/portTICK_PERIOD_MS);
//            TMC7300_read_register(IFCNT, &value);
//            execute_cmd(SET_PWM_A, WRITE_CMD, i);
//            vTaskDelay(50/portTICK_PERIOD_MS);
