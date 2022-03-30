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
#include "event_groups.h"

//==============================================================================
// Global data
//==============================================================================

// Hardware

const uint LED_PIN = PICO_DEFAULT_LED_PIN;
const uint LOG_PIN = GP2;
const uint BLINK_PIN = LED_PIN;

// FreeRTOS components handles

TaskHandle_t taskhndl_Task_Robokid;
TaskHandle_t taskhndl_Task_read_sensors;
TaskHandle_t taskhndl_Task_read_gamepad;
TaskHandle_t taskhndl_Task_display_LCD;
TaskHandle_t taskhndl_Task_drive_motors;
TaskHandle_t taskhndl_Task_error;
TaskHandle_t taskhndl_Task_sounder;
TaskHandle_t taskhndl_Task_blink_LED;

SemaphoreHandle_t semaphore_LCD_data;
SemaphoreHandle_t semaphore_SSD1306_display;
SemaphoreHandle_t semaphore_system_IO_data;
SemaphoreHandle_t semaphore_system_status;
SemaphoreHandle_t semaphore_gamepad_data;
SemaphoreHandle_t semaphore_tune_data;

QueueHandle_t queue_motor_cmds;
QueueHandle_t queue_error_messages;

EventGroupHandle_t eventgroup_push_buttons;

// Task data

struct task_data_s     task_data[NOS_TASKS];

// System data structures. Protected with MUTEXES

struct system_IO_data_s    system_IO_data;
struct gamepad_data_s      gamepad_data;
struct system_status_s     system_status;
struct tune_data_s    tune_data;

struct font_data_s         font_data[NOS_FONTS] = {
    {Terminal_9x16,                (SSD1306_LCDWIDTH / TERMINAL_9x16_FONT_WIDTH)},              // font 0
    {robokid_LCD_icons_font_15x16, (SSD1306_LCDWIDTH / ROBOKID_LCD_ICONS_FONT_15x16_WIDTH)},    // font 1
};
struct LCD_row_data_s      LCD_row_data[SS1306_NOS_LCD_ROWS];

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
        for (index=0; index < NOS_ROBOKID_PUSH_BUTTONS ; index++ ) {
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
    // LCD row data
    for (index=0 ; index < SS1306_NOS_LCD_ROWS ; index++) {
            LCD_row_data[index].dirty_bit   = false;
            LCD_row_data[index].font        = 0;
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

    xTaskCreate(Task_display_LCD,
                "Task_display_LCD",
                configMINIMAL_STACK_SIZE,
                NULL,
                TASK_PRIORITYNORMAL,
                &taskhndl_Task_display_LCD
    );

    xTaskCreate(Task_read_gamepad,
                "Task_read_gamepad",
                configMINIMAL_STACK_SIZE,
                NULL,
                TASK_PRIORITYNORMAL,
                &taskhndl_Task_read_gamepad
    );

    xTaskCreate(Task_sounder,
                "sounder_task",
                configMINIMAL_STACK_SIZE,
                NULL,
                TASK_PRIORITYIDLE,
                &taskhndl_Task_sounder
    );  

    xTaskCreate(Task_error,
                "Task_error",
                configMINIMAL_STACK_SIZE,
                NULL,
                TASK_PRIORITYIDLE,
                &taskhndl_Task_error
    );

    xTaskCreate(Task_blink_LED,
                "blink_task",
                configMINIMAL_STACK_SIZE,
                NULL,
                TASK_PRIORITYIDLE,
                &taskhndl_Task_blink_LED
    );

    semaphore_LCD_data          = xSemaphoreCreateMutex();
    semaphore_SSD1306_display   = xSemaphoreCreateMutex();
    semaphore_system_IO_data    = xSemaphoreCreateMutex();
    semaphore_system_status     = xSemaphoreCreateMutex();
    semaphore_gamepad_data      = xSemaphoreCreateMutex();
    semaphore_tune_data         = xSemaphoreCreateMutex();

    queue_motor_cmds     = xQueueCreate(MOTOR_CMD_QUEUE_LENGTH, sizeof(struct motor_cmd_packet_s));   
    queue_error_messages = xQueueCreate(ERROR_MESSAGE_QUEUE_LENGTH, sizeof(struct error_message_s));

    eventgroup_push_buttons = xEventGroupCreate (); 

    vTaskStartScheduler();

    HANG;

    return 0;
}

