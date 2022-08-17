/**
 * @file    main.c
 * @author  Jim Herd
 * @brief   Development program for Robokid based on 
 *          RP2040/FreeRTOS/TinyUSB/1306 LCD/DRV8833 H-bridge
 */

#include "system.h"
#include "common.h"

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/binary_info.h"
#include "hardware/adc.h"

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
TaskHandle_t taskhndl_Task_serial_output;
TaskHandle_t taskhndl_Task_log_system;
TaskHandle_t taskhndl_Task_blink_LED;

SemaphoreHandle_t semaphore_LCD_data;
SemaphoreHandle_t semaphore_SSD1306_display;
SemaphoreHandle_t semaphore_system_IO_data;
SemaphoreHandle_t semaphore_system_status;
SemaphoreHandle_t semaphore_gamepad_data;
SemaphoreHandle_t semaphore_tune_data;

QueueHandle_t queue_motor_cmds;
QueueHandle_t queue_error_messages;
QueueHandle_t queue_print_string_buffers;
QueueHandle_t queue_free_buffers;

EventGroupHandle_t eventgroup_push_buttons;

//==============================================================================
// System data structures. Protected with MUTEXES

struct system_IO_data_s    system_IO_data;
struct gamepad_data_s      gamepad_data;
struct system_status_s     system_status;
struct tune_data_s         tune_data;

struct error_data_s        error_data;

struct font_data_s         font_data[NOS_FONTS] = {
    {Terminal_9x16,                (SSD1306_LCDWIDTH / TERMINAL_9x16_FONT_WIDTH)},              // font 0
    {robokid_LCD_icons_font_15x16, (SSD1306_LCDWIDTH / ROBOKID_LCD_ICONS_FONT_15x16_WIDTH)},    // font 1
};
struct LCD_row_data_s      LCD_row_data[SS1306_NOS_LCD_ROWS];

// ASCII string buffers

char print_string_buffers[NOS_PRINT_STRING_BUFFERS][STRING_LENGTH];

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

    // system modes
        system_IO_data.robokid_modes.primary_mode   = PRIMARY_NULL_MODE;
        system_IO_data.robokid_modes.secondary_mode = SECONDARY_NULL_MODE;
        system_IO_data.robokid_modes.tertiary_mode  = TERTIARY_NULL_MODE;
    // system status
        system_status.error_state = OK;
    // Battery voltage
        system_IO_data.system_voltage = 500;
    // CD4051 channel data
        for (index=0; index < NOS_CD4051_CHANNELS ; index++ ) {
            system_IO_data.analogue_global_data[index].active = true;
            system_IO_data.analogue_global_data[index].apply_filter = true;
            system_IO_data.analogue_global_data[index].raw.current_value = 0;
            system_IO_data.analogue_global_data[index].raw.percent_current_value = 0;
            system_IO_data.analogue_global_data[index].raw.glitch_threshold = A_D_GLITCH_THRESHOLD;
            system_IO_data.analogue_global_data[index].raw.glitch_error_count_threshold = GLITCH_COUNT_THRESHOLD;
            system_IO_data.analogue_global_data[index].processed.channel_type = ANALOGUE_TYPE;
            system_IO_data.analogue_global_data[index].processed.value = 0;
        }
    // USB data
        gamepad_data.state = DISABLED;
        gamepad_data.vid = 0; gamepad_data.pid = 0;
        gamepad_data.dpad_x = GAMEPAD_DPAD_X_AXIS_NULL; 
        gamepad_data.dpad_y = GAMEPAD_DPAD_Y_AXIS_NULL;
        gamepad_data.button_X = 0; gamepad_data.button_Y = 0;
        gamepad_data.button_A = 0; gamepad_data.button_B = 0;
        gamepad_data.button_L = 0; gamepad_data.button_R = 0;
        gamepad_data.button_START = 0; gamepad_data.button_SELECT = 0;
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
            system_IO_data.LED_data[index].state         = LED_OFF;
            system_IO_data.LED_data[index].flash         = false;
            system_IO_data.LED_data[index].flash_value   = false;
            system_IO_data.LED_data[index].flash_time    = DEFAULT_FLASH_TIME;
            system_IO_data.LED_data[index].flash_counter = 0;   
        };
        system_IO_data.LED_data[0].pin_number = LED_A_PIN;
        system_IO_data.LED_data[1].pin_number = LED_B_PIN;
        system_IO_data.LED_data[2].pin_number = LED_C_PIN;
        system_IO_data.LED_data[3].pin_number = LED_D_PIN;
    // Floor sensor data
        for (index=0; index < NOS_ROBOKID_LINE_SENSORS ; index++ ) {
            system_IO_data.line_sensor_data[index].percent_value = 0;
            system_IO_data.line_sensor_data[index].threshhold = 50;
            system_IO_data.line_sensor_data[index].binary_value = 0;
        };
    // Vehicle data
        system_IO_data.vehicle_data.vehicle_state = STOPPED;
        system_IO_data.vehicle_data.speed = 0;
    // Task execution data
        for (index=0 ; index < NOS_TASKS ; index++) {
            system_IO_data.task_data[index].priority = TASK_PRIORITYNORMAL;
            system_IO_data.task_data[index].last_exec_time    = 0;
            system_IO_data.task_data[index].lowest_exec_time  = UINT32_MAX;
            system_IO_data.task_data[index].highest_exec_time = 0;
        };
    // LCD row data
    for (index=0 ; index < SS1306_NOS_LCD_ROWS ; index++) {
            LCD_row_data[index].dirty_bit   = false;
            LCD_row_data[index].font        = 0;
            LCD_row_data[index].invert      = false;
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

    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    uart_puts(UART_ID, "Robokid 2\n");

    adc_init();

    init_system_data();
 
    xTaskCreate(Task_Robokid,
                "Robokid_task",
                1024,  // configMINIMAL_STACK_SIZE,
                NULL,
                TASK_PRIORITYLOW,
                &taskhndl_Task_Robokid
    );
    system_IO_data.task_data[TASK_ROBOKID].task_handle = taskhndl_Task_Robokid;

    xTaskCreate(Task_drive_motors,
                "Drive_motors_task",
                configMINIMAL_STACK_SIZE,
                NULL,
                TASK_PRIORITYNORMAL,
                &taskhndl_Task_drive_motors
    );
    system_IO_data.task_data[TASK_DRIVE_MOTORS].task_handle = taskhndl_Task_drive_motors;

    xTaskCreate(Task_read_sensors,
                "Read_sensors_task",
                1024, // configMINIMAL_STACK_SIZE,
                NULL,
                TASK_PRIORITYNORMAL,
                &taskhndl_Task_read_sensors
    );
    system_IO_data.task_data[TASK_READ_SENSORS].task_handle = taskhndl_Task_read_sensors;

    xTaskCreate(Task_display_LCD,
                "Display_LCD_task",
                configMINIMAL_STACK_SIZE,
                NULL,
                TASK_PRIORITYBELOWNORMAL,
                &taskhndl_Task_display_LCD
    );
    system_IO_data.task_data[TASK_DISPLAY].task_handle = taskhndl_Task_display_LCD;

    xTaskCreate(Task_read_gamepad,
                "Read_gamepad_task",
                1024,    // configMINIMAL_STACK_SIZE,
                NULL,
                TASK_PRIORITYNORMAL,
                &taskhndl_Task_read_gamepad
    );
    system_IO_data.task_data[TASK_READ_GAMEPAD].task_handle = taskhndl_Task_read_gamepad;

    xTaskCreate(Task_sounder,
                "Sounder_task",
                configMINIMAL_STACK_SIZE,
                NULL,
                TASK_PRIORITYIDLE,
                &taskhndl_Task_sounder
    );
    system_IO_data.task_data[TASK_SOUNDER].task_handle = taskhndl_Task_sounder;

    xTaskCreate(Task_error,
                "Error_task",
                configMINIMAL_STACK_SIZE,
                NULL,
                TASK_PRIORITYIDLE,
                &taskhndl_Task_error
    );
    system_IO_data.task_data[TASK_ERROR].task_handle = taskhndl_Task_error;

    xTaskCreate(Task_serial_output,
                "Send ASCII strings to serial port",
                1024,  // configMINIMAL_STACK_SIZE,
                NULL,
                TASK_PRIORITYIDLE,
                &taskhndl_Task_serial_output
    );
    system_IO_data.task_data[TASK_SERIAL_OUTPUT].task_handle = taskhndl_Task_serial_output;

    xTaskCreate(Task_log_system_data,   
                "Log task data",
                configMINIMAL_STACK_SIZE,
                NULL,
                TASK_PRIORITYIDLE,
                &taskhndl_Task_log_system
    );
    system_IO_data.task_data[TASK_LOG].task_handle = taskhndl_Task_log_system;

    xTaskCreate(Task_blink_LED,
                "Blink_task",
                configMINIMAL_STACK_SIZE,
                NULL,
                TASK_PRIORITYIDLE,
                &taskhndl_Task_blink_LED
    );
    system_IO_data.task_data[TASK_BLINK].task_handle = taskhndl_Task_blink_LED;

    semaphore_LCD_data          = xSemaphoreCreateMutex();
    semaphore_SSD1306_display   = xSemaphoreCreateMutex();
    semaphore_system_IO_data    = xSemaphoreCreateMutex();
    semaphore_system_status     = xSemaphoreCreateMutex();
    semaphore_gamepad_data      = xSemaphoreCreateMutex();
    semaphore_tune_data         = xSemaphoreCreateMutex();

    queue_motor_cmds     = xQueueCreate(MOTOR_CMD_QUEUE_LENGTH, sizeof(struct motor_cmd_packet_s));   
    queue_error_messages = xQueueCreate(ERROR_MESSAGE_QUEUE_LENGTH, sizeof(struct error_message_s));
    queue_print_string_buffers = xQueueCreate(NOS_PRINT_STRING_BUFFERS+1, sizeof(struct string_buffer_s));
    queue_free_buffers   = xQueueCreate(NOS_PRINT_STRING_BUFFERS+1, sizeof(struct string_buffer_s));

    eventgroup_push_buttons = xEventGroupCreate (); 

    prime_free_buffer_queue();

    vTaskStartScheduler();

    HANG;

    return 0;
}

