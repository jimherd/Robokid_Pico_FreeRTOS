/**
 * @file    system.h
 * @author  Jim Herd 
 * @brief   Constants relating to Robokid robot
 * @version 0.1
 * @date    2022-01-09
 */

#include    "pico/stdlib.h"
#include    "Pico_IO.h"

#include    "FreeRTOS.h"
#include    "semphr.h"      // from FreeRTOS

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

//==============================================================================
// Constants
//==============================================================================
// Robokid parameters
#define     NOS_ROBOKID_MOTORS          2
#define     NOS_ROBOKID_SWITCHES        4
#define     NOS_ROBOKID_LEDS            4
#define     NOS_ROBOKID_FLOOR_SENSORS   2

#define     LEFT_MOTOR_FLIP_MODE        false
#define     RIGHT_MOTOR_FLIP_MODE       true

//==============================================================================
// TMC7300 H-bridge
#define     TMC7300_UART_PORT   uart1  
#define     BAUD_RATE           115200   

#define     UART_TX_PIN         GP8         // Pin 11
#define     UART_RX_PIN         GP9         // Pin 12
#define     TMC7300_EN_PIN      GP10        // Pin 14

//==============================================================================
// DRV8833 H-bridge
#define     LEFT_MOTOR_CONTROL_PIN_A    GP18  // Pin 18 : PWM slice 1, channel A
#define     LEFT_MOTOR_CONTROL_PIN_B    GP19  // Pin 19 : PWM slice 1, channel B

#define     RIGHT_MOTOR_CONTROL_PIN_A   GP20  // Pin 20 : PWM slice 2, channel A
#define     RIGHT_MOTOR_CONTROL_PIN_B   GP21  // Pin 21 : PWM slice 2, channel B

typedef enum {FULL_SPEED=100, HALF_SPEED=50} speed_t;
typedef enum {STOPPED, MOVING_FORWARD, MOVING_BACKWARD, TURNING_LEFT, TURNING_RIGHT} vehicle_state_t;
typedef enum {LEFT_MOTOR, RIGHT_MOTOR} motor_t;
typedef enum {MODE_INIT, MODE_RUNNING, MODE_STOPPED} mode_state_t;
typedef enum {MOTOR_OFF, MOTOR_BRAKE, MOVE} motor_cmd_t;

typedef enum {OFF, FORWARD, BACKWARD} direction_t;
typedef enum {NO_FLIP, FLIP} motor_orientation_t;
typedef enum {LOW, HIGH, PWM} DRV8833_in_t;

//==============================================================================
// 4 push switches + 4 LEDs

#define     SWITCH_0_PIN    GP12
#define     SWITCH_1_PIN    GP13
#define     SWITCH_2_PIN    GP14
#define     SWITCH_3_PIN    GP15

#define     SWITCH_0_MASK           (1 << SWITCH_0_PIN)
#define     SWITCH_1_MASK           (1 << SWITCH_1_PIN)
#define     SWITCH_2_MASK           (1 << SWITCH_2_PIN)
#define     SWITCH_3_MASK           (1 << SWITCH_3_PIN)
#define     SWITCH_MASK             (SWITCH_0_MASK | SWITCH_1_MASK | SWITCH_2_MASK | SWITCH_3_MASK)
#define     NOS_SWITCH_SAMPLES      3

#define     LED_0_PIN       GP16
#define     LED_1_PIN       GP17
#define     LED_2_PIN       GP27
#define     LED_3_PIN       GP28

//==============================================================================
// SNES Gamepad
#define     GENERIC_GAMEPAD_VID     0x081F
#define     GENERIC_GAMEPAD_PID     0xE401

#define     GAMEPAD_DPAD_X_AXIS_LEFT    0x00
#define     GAMEPAD_DPAD_X_AXIS_NULL    0x7F
#define     GAMEPAD_DPAD_X_AXIS_RIGHT   0xFF

#define     GAMEPAD_DPAD_Y_AXIS_UP      0x00
#define     GAMEPAD_DPAD_Y_AXIS_NULL    0x7F
#define     GAMEPAD_DPAD_Y_AXIS_DOWN    0xFF

enum  gamepad_state {DISABLED, ENABLED} ;
enum  gamepad_switch_state {released, pressed};
enum  gamepad_dpad_X_axis {X_AXIS_OFF, X_AXIS_LEFT, X_AXIS_RIGHT};
enum  gamepad_dpad_Y_axis {Y_AXIS_OFF, Y_AXIS_UP, Y_AXIS_DOWN};

//==============================================================================
// SSD1306 display

#define SSD1306_LCDWIDTH 128
#define SSD1306_LCDHEIGHT 64

#define     SSD1306_SPI_SPEED      8000000   // SSD1306 SPIMax=10MHz

//==============================================================================
// errors

typedef enum {
    OK                  =  0,
    FAULT               = -1,
} error_codes_t;

//==============================================================================
// Freertos

typedef enum TASKS {TASK_ROBOKID, TASK_DRIVE_MOTORS, TASK_READ_SENSORS, 
                TASK_DISPLAY, TASK_READ_GAMEPAD, TASK_BLINK} task_t;

#define     NOS_TASKS   (TASK_BLINK - TASK_ROBOKID + 1)

#define     Task_read_sensors_frequency                 50  //Hz
#define     Task_read_sensors_frequency_tick_count      ((1000/Task_read_sensors_frequency) * portTICK_PERIOD_MS)

#define     MOTOR_CMD_QUEUE_LENGTH     8

//==============================================================================
// Set of 8 priority levels (set 8 in FreeRTOSconfig.h)
//==============================================================================

#define   TASK_PRIORITYIDLE             0
#define   TASK_PRIORITYLOW              1
#define   TASK_PRIORITYBELOWNORMAL      2
#define   TASK_PRIORITYNORMAL           3
#define   TASK_PRIORITYABOVENORMAL      4
#define   TASK_PRIORITYHIGH             5
#define   TASK_PRIORITYREALTIME         6
#define   TASK_PRIORITYERROR            7

//==============================================================================
// Macros
//==============================================================================

#define     START_PULSE                 gpio_put(LOG_PIN, 1)
#define     STOP_PULSE                  gpio_put(LOG_PIN, 0)

#define     DISABLE_POWER_STAGE          gpio_put(TMC7300_EN_PIN, 0)
#define     ENABLE_POWER_STAGE           gpio_put(TMC7300_EN_PIN, 1)

#define     FOREVER     for(;;)
#define     HANG        for(;;)

#define     ATTRIBUTE_PACKED     __attribute__ ((__packed__))

//==============================================================================
// definitions of system data structures
//==============================================================================
/**
 * @brief data structure for report returned from gamepad
 * 
 */
typedef struct TU_ATTR_PACKED  {    // packed attribute
    uint8_t     dpad_x, dpad_y;         // dpad returned as 8-bit values
    uint8_t     dummy2, dummy3,dummy4;

    struct {
        uint8_t dummy5      :4;
        uint8_t button_X    :1;
        uint8_t button_A    :1;
        uint8_t button_B    :1;
        uint8_t button_Y    :1;
    };

    struct {
        uint8_t button_L      :1;
        uint8_t button_R      :1;
        uint8_t dummy6        :2;
        uint8_t button_START  :1;
        uint8_t button_SELECT :1;
        uint8_t dummy7        :2;
    };
    uint8_t     dummy8;
} SNES_gamepad_report_t;

//==============================================================================
/**
 * @brief data structure to hold current gamepad information
 * 
 */
typedef struct ATTRIBUTE_PACKED {
    uint8_t     state;                  // ENABLED or DISABLED
    uint32_t    vid, pid;
    uint8_t     dpad_x, dpad_y;
    uint8_t     button_X, button_Y, button_A, button_B;
    uint8_t     button_L, button_R;
    uint8_t     button_START, button_SELECT;
} gamepad_data_t;

typedef struct {
    int8_t     error_state;
} system_status_t;

typedef union {
    uint32_t    command;
    struct  {
        motor_cmd_t   cmd;
        int8_t        param1, param2, param3;
    };
} motor_cmd_packet_t;

typedef struct {
    direction_t     motor_state;
    uint8_t         pwm_width;
    bool            flip;
} motor_data_t;

typedef struct  {
        bool      switch_value;
        uint32_t  on_time;
} push_button_data_t;

typedef struct {
    vehicle_state_t     vehicle_state;
    uint32_t            speed;
} vehicle_data_t;

typedef struct {
    uint8_t   pin_number;
    bool      value;
    bool      flash;
    uint8_t   flash_time;    // units of 20mS
    uint8_t   flash_counter;
    bool      flash_value;
} LED_data_t;

typedef struct {
    uint8_t     raw_value;
    uint8_t     threshhold;
    bool        binary_value;
} floor_sensor_data_t;

//==============================================================================
/**
 * @brief   Central store of system data. Access by mutex.
 */
typedef struct  {
    system_status_t     system_status;      // error codes
    uint16_t            system_voltage;
    motor_data_t        motor_data[NOS_ROBOKID_MOTORS];
    LED_data_t          LED_data[NOS_ROBOKID_LEDS];
    push_button_data_t  push_button_data[NOS_ROBOKID_SWITCHES];
    floor_sensor_data_t   floor_sensor_data[NOS_ROBOKID_FLOOR_SENSORS];
    vehicle_data_t      vehicle_data;
} system_IO_data_t;

//==============================================================================
typedef struct {
    uint8_t     priority;
    struct {
        uint32_t    last_exec_time;
        uint32_t    lowest_exec_time;
        uint32_t    highest_exec_time;
    };
} task_data_t;

//==============================================================================
// Extern references
//==============================================================================
// System data structures

extern task_data_t task_data[];

extern gamepad_data_t   gamepad_data;
extern system_status_t  system_status;
extern system_IO_data_t system_IO_data;

extern const uint8_t *error_strings[];

// Hardware

extern const uint LED_PIN;
extern const uint LOG_PIN;
extern const uint BLINK_PIN;

// FreeRTOS components

extern void Task_Robokid(void *p);
extern void Task_read_sensors(void *p);
extern void Task_read_gamepad(void *p);             // tasks
extern void Task_blink_LED(void *p);
extern void Task_display_LCD(void *p);
extern void Task_drive_motors(void *p);

extern SemaphoreHandle_t semaphore_system_IO_data;
extern SemaphoreHandle_t semaphore_system_status;   //semaphores
extern SemaphoreHandle_t semaphore_gamepad_data;

extern QueueHandle_t queue_motor_cmds;              // queues

// SSD1306 LCD Fonts

extern const unsigned char Terminal_9x16[];
extern const unsigned char robokid_LCD_icons_font_15x16[];

extern const uint8_t *font_table[];
extern const unsigned char Terminal_12x16[];
extern const unsigned char Font_6x8[];
extern const unsigned char Segment_25x40[];
extern const unsigned char truck_bmp[1024];

#endif /* __SYSTEM_H__ */