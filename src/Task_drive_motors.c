
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

motor_cmd_t  command;
uint32_t     value;

    //TMC7300_Init();             // to power-on condition
    DRV8833_init();

    //execute_cmd(ENABLE_MOTORS, WRITE_CMD, 0);
    FOREVER {
    //    xQueueReceive(queue_motor_cmds, &command,  portMAX_DELAY);
        for (int32_t i=-100 ; i<101 ; i++) {
            DRV8833_set_motor(LEFT_MOTOR, MOVE, i);
            DRV8833_set_motor(RIGHT_MOTOR, MOVE, i);
            vTaskDelay(50/portTICK_PERIOD_MS);
        }
        vTaskDelay(3000/portTICK_PERIOD_MS);
    }
}

//            vTaskDelay(1/portTICK_PERIOD_MS);
//            TMC7300_read_register(IFCNT, &value);
//            execute_cmd(SET_PWM_A, WRITE_CMD, i);
//            vTaskDelay(50/portTICK_PERIOD_MS);
