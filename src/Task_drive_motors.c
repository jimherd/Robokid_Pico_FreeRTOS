
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


//==============================================================================
// Main task routine
//==============================================================================
void Task_drive_motors(void *p) {

motor_cmd_t  command;
uint32_t     value;

    TMC7300_Init();             // to power-on condition

    //execute_cmd(ENABLE_MOTORS, WRITE_CMD, 0);
    FOREVER {
    //    xQueueReceive(queue_motor_cmds, &command,  portMAX_DELAY);
        for (uint32_t i=0 ; i<100 ; i++) {
            vTaskDelay(5/portTICK_PERIOD_MS);
            TMC7300_read_register(IOIN, &value);
//            execute_cmd(SET_PWM_A, WRITE_CMD, 50);
            vTaskDelay(50/portTICK_PERIOD_MS);
        }
        START_PULSE;
        vTaskDelay(3000/portTICK_PERIOD_MS);
        STOP_PULSE;
    }
}
