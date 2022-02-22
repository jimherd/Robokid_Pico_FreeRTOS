#include "system.h"

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

//==============================================================================
// Main task routine
//==============================================================================
void Task_Robokid(void *p) {

    FOREVER {

        vTaskDelay(100/portTICK_PERIOD_MS);
    }
}