
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

    TMC7300_Init();

    FOREVER {
        xQueueReceive(queue_motor_cmds, &command,  portMAX_DELAY);

        
    }
}