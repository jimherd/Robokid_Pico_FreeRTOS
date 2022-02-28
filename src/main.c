/**
 * @file    main.c
 * @author  Jim Herd
 * @brief   Development program for Robokid based on 
 *          Pico/FreeRTOS/TinyUSB/1306 LCD/Trinamic H-bridge
 *
 * @date    2022-01-06
 */

#include "system.h"

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/binary_info.h"

#include  "Pico_IO.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

//==============================================================================
// Global data
//==============================================================================

// Hardware

const uint LED_PIN = PICO_DEFAULT_LED_PIN;
const uint LOG_PIN = GP2;
const uint BLINK_PIN = LED_PIN;

// FreeRTOS components and task data

TaskHandle_t taskhndl_Task_Robokid;
TaskHandle_t taskhndl_Task_read_sensors;
TaskHandle_t taskhndl_Task_read_gamepad;
TaskHandle_t taskhndl_Task_display_LCD;
TaskHandle_t taskhndl_Task_drive_motors;
TaskHandle_t taskhndl_Task_blink_LED;

SemaphoreHandle_t semaphore_system_IO_data;
SemaphoreHandle_t semaphore_system_status;
SemaphoreHandle_t semaphore_gamepad_data ;

QueueHandle_t queue_motor_cmds;

task_data_t     task_data[NOS_TASKS];

// System data structures. Protected with MUTEXES

system_IO_data_t    system_IO_data;
gamepad_data_t      gamepad_data;
system_status_t     system_status;

//==============================================================================
// System initiation
//==============================================================================
/**
 * @brief set central data store to power-on values
 * 
 */
void init_system_data(void)
{
uint8_t     index;

    // system status
        system_status.error_state = OK;
    // Battery voltage
        system_IO_data.system_voltage = 500;
    // Motor data
        system_IO_data.motor_data[LEFT_MOTOR].motor_state = OFF;
        system_IO_data.motor_data[LEFT_MOTOR].pwm_width = 0;
        system_IO_data.motor_data[LEFT_MOTOR].flip  = LEFT_MOTOR_FLIP_MODE;
        system_IO_data.motor_data[RIGHT_MOTOR].motor_state = OFF;
        system_IO_data.motor_data[RIGHT_MOTOR].pwm_width = 0;
        system_IO_data.motor_data[RIGHT_MOTOR].flip = RIGHT_MOTOR_FLIP_MODE;
    // Push button data
        for (index=0; index < NOS_ROBOKID_SWITCHES ; index++ ) {
            system_IO_data.push_button_data[index].switch_value = false;
            system_IO_data.push_button_data[index].on_time = 0;
        };
    // LED data
        for (index=0; index < NOS_ROBOKID_LEDS ; index++ ) {
            system_IO_data.LED_data[index].value         = false;
            system_IO_data.LED_data[index].flash         = false;
            system_IO_data.LED_data[index].flash_value   = false;
            system_IO_data.LED_data[index].flash_time    = 0;
            system_IO_data.LED_data[index].flash_counter = 0;   
        };
        system_IO_data.LED_data[0].pin_number = LED_0_PIN;
        system_IO_data.LED_data[1].pin_number = LED_1_PIN;
        system_IO_data.LED_data[2].pin_number = LED_2_PIN;
        system_IO_data.LED_data[3].pin_number = LED_3_PIN;
    // Floor sensor data
        for (index=0; index < NOS_ROBOKID_FLOOR_SENSORS ; index++ ) {
            system_IO_data.floor_sensor_data[index].raw_value = 0;
            system_IO_data.floor_sensor_data[index].threshhold = 128;
            system_IO_data.floor_sensor_data[index].threshhold = false;
        };
    // Vehicle data
        system_IO_data.vehicle_data.vehicle_state = STOPPED;
        system_IO_data.vehicle_data.speed = 0;
    // Task execution data
        for (index=0 ; index < NOS_TASKS ; index++) {
            task_data[index].priority = TASK_PRIORITYNORMAL;
            task_data[index].last_exec_time    = 0;
            task_data[index].lowest_exec_time  = UINT32_MAX;
            task_data[index].highest_exec_time = 0;
        };
}
//==============================================================================
/**
 * @brief   Initialise datastore, some hardware, and FreeRTOS elements
 * 
 * @return int 
 */
int main() 
{
    stdio_init_all();

    gpio_init(LOG_PIN);
    gpio_set_dir(LOG_PIN, GPIO_OUT);
    gpio_pull_down(LOG_PIN);         // should default but just make sure

    init_system_data();
    
    system_status.error_state = -26;
 
    xTaskCreate(Task_Robokid,
                "Robokid_task",
                configMINIMAL_STACK_SIZE,
                NULL,
                TASK_PRIORITYIDLE,
                &taskhndl_Task_Robokid
    );

    xTaskCreate(Task_read_gamepad,
                "Task_read_gamepad",
                configMINIMAL_STACK_SIZE,
                NULL,
                TASK_PRIORITYNORMAL,
                &taskhndl_Task_read_gamepad
    );

    xTaskCreate(Task_display_LCD,
                "Task_display_LCD",
                configMINIMAL_STACK_SIZE,
                NULL,
                TASK_PRIORITYNORMAL,
                &taskhndl_Task_display_LCD
    );

    xTaskCreate(Task_drive_motors,
                "Task_drive_motors",
                configMINIMAL_STACK_SIZE,
                NULL,
                TASK_PRIORITYNORMAL,
                &taskhndl_Task_drive_motors
    );

    xTaskCreate(Task_read_sensors,
                "Task_read_sensors",
                configMINIMAL_STACK_SIZE,
                NULL,
                TASK_PRIORITYNORMAL,
                &taskhndl_Task_read_sensors
    );

     xTaskCreate(Task_blink_LED,
                "blink_task",
                configMINIMAL_STACK_SIZE,
                NULL,
                TASK_PRIORITYIDLE,
                &taskhndl_Task_blink_LED
    );

    semaphore_system_IO_data    = xSemaphoreCreateMutex();
    semaphore_system_status     = xSemaphoreCreateMutex();
    semaphore_gamepad_data      = xSemaphoreCreateMutex();

    queue_motor_cmds = xQueueCreate(MOTOR_CMD_QUEUE_LENGTH, sizeof(motor_cmd_packet_t));   

    vTaskStartScheduler();

    HANG;

    return 0;
}

