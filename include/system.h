/**
 * @file    system.h
 * @author  Jim Herd 
 * @brief   Constants relating to Robokid robot
 */

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include    "pico/stdlib.h"
#include    "Pico_IO.h"

#include    "FreeRTOS.h"
#include    "semphr.h"
#include    "event_groups.h"

#include    "error_codes.h"

//==============================================================================
// Version number
//==============================================================================
#define     MAJOR_VERSION       0
#define     MINOR_VERSION       1
#define     PATCH_VERSION       0

//==============================================================================
// Complie time configuration
//==============================================================================

//#define SSD1306_INTERFACE_SPI
#define SSD1306_INTERFACE_I2C

// #define PUSH_SWITCH_DEFAULT_LOW
 #define PUSH_SWITCH_DEFAULT_HIGH

//==============================================================================
// Constants
//==============================================================================
// CPU
#define     CPU_CLOCK_FREQUENCY         125000000   // 125MHz

//==============================================================================
// Useful times

#define     HALF_SECOND      (500/portTICK_PERIOD_MS)
#define     ONE_SECOND       (1000/portTICK_PERIOD_MS)
#define     TWO_SECONDS      (2000/portTICK_PERIOD_MS)

//==============================================================================
// Robokid parameters
#define     NOS_ROBOKID_MOTORS          2
#define     NOS_ROBOKID_PUSH_BUTTONS    4
#define     NOS_ROBOKID_LEDS            4
#define     NOS_ROBOKID_LINE_SENSORS    3

#define     LEFT_MOTOR_FLIP_MODE        true
#define     RIGHT_MOTOR_FLIP_MODE       false

//==============================================================================
// Serial comms port (UART)

#define UART_TX_PIN GP0
#define UART_RX_PIN GP1

#define UART_ID uart0
#define BAUD_RATE 115200

//==============================================================================
// DRV8833 H-bridge

#define MOTOR_COUNT_FREQ        25000000    // 25MHz
#define MOTOR_PWM_FREQ          5000        // 5KHz

#define MOTOR_PWM_CLK_DIV       (CPU_CLOCK_FREQUENCY / MOTOR_COUNT_FREQ)
#define MOTOR_PWM_MAX_COUNT     (MOTOR_COUNT_FREQ / MOTOR_PWM_FREQ)
#define MOTOR_PWM_MIN_COUNT     0

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

#define     ZERO_CROSS_OVER_DELAY_MS    (10/portTICK_PERIOD_MS)

//==============================================================================
// 4 push switches + 4 LEDs

typedef enum {PUSH_BUTTON_A, PUSH_BUTTON_B, PUSH_BUTTON_C, PUSH_BUTTON_D} push_buttons_te;

#define     PUSH_BUTTON_A_PIN    GP12
#define     PUSH_BUTTON_B_PIN    GP13
#define     PUSH_BUTTON_C_PIN    GP14
#define     PUSH_BUTTON_D_PIN    GP15

#define     PUSH_BUTTON_A_HARDWARE_MASK          (1 << PUSH_BUTTON_A_PIN)
#define     PUSH_BUTTON_B_HARDWARE_MASK          (1 << PUSH_BUTTON_B_PIN)
#define     PUSH_BUTTON_C_HARDWARE_MASK          (1 << PUSH_BUTTON_C_PIN)
#define     PUSH_BUTTON_D_HARDWARE_MASK          (1 << PUSH_BUTTON_D_PIN)
#define     PUSH_BUTTON_HARDWARE_MASK         (PUSH_BUTTON_A_HARDWARE_MASK | PUSH_BUTTON_B_HARDWARE_MASK | PUSH_BUTTON_C_HARDWARE_MASK | PUSH_BUTTON_D_HARDWARE_MASK)

#define     PUSH_BUTTON_ON_EVENT_MASK   ((1 << NOS_ROBOKID_PUSH_BUTTONS) - 1)
#define     PUSH_BUTTON_OFF_EVENT_MASK  (PUSH_BUTTON_ON_EVENT_MASK << NOS_ROBOKID_PUSH_BUTTONS)

#define     NOS_SWITCH_SAMPLES          3

#define     PUSH_BUTTON_A_EVENT_BIT     0
#define     PUSH_BUTTON_B_EVENT_BIT     (PUSH_BUTTON_A_EVENT_BIT + 1)
#define     PUSH_BUTTON_C_EVENT_BIT     (PUSH_BUTTON_A_EVENT_BIT + 2)
#define     PUSH_BUTTON_D_EVENT_BIT     (PUSH_BUTTON_A_EVENT_BIT + 3)

#define     PUSH_BUTTON_A_EVENT_MASK    (1 << PUSH_BUTTON_A)
#define     PUSH_BUTTON_B_EVENT_MASK    (1 << PUSH_BUTTON_B)
#define     PUSH_BUTTON_C_EVENT_MASK    (1 << PUSH_BUTTON_C)
#define     PUSH_BUTTON_D_EVENT_MASK    (1 << PUSH_BUTTON_D)

typedef enum {LED_NO_CHANGE, LED_OFF, LED_FLASH, LED_ON} LED_state_te;

enum {LED_A, LED_B, LED_C, LED_D};

#define     DEFAULT_FLASH_TIME      10

#define     LED_A_PIN       GP16
#define     LED_B_PIN       GP17
#define     LED_C_PIN       GP27
#define     LED_D_PIN       GP28

//==============================================================================
// 3+8 channel analogue input system
// 3 RP2040 channels + 1 RP2040 channel fed by CD4051 8-line multiplexer

#define NOS_RP2040_CHANNELS     3
#define NOS_CD4051_CHANNELS     8

#define CD4051_ADDRESS_A_PIN       GP4
#define CD4051_ADDRESS_B_PIN       GP5
#define CD4051_ADDRESS_C_PIN       GP6

#define CD4051_RP2040_channel       0

#define CD4051_ADDRESS_MASK    ((1 << CD4051_ADDRESS_A_PIN) + \
                                (1 << CD4051_ADDRESS_B_PIN) + \
                                (1 << CD4051_ADDRESS_C_PIN))

#define ANALOGUE_CD4051_INPUT_CHANNEL       GP26

// CD4051 channel allocation (8 inputs)

#define POT_A_channel               0
#define POT_B_channel               1
#define POT_C_channel               7
#define LINE_SENSOR_RIGHT_CHANNEL   4
#define LINE_SENSOR_MID_CHANNEL     5
#define LINE_SENSOR_LEFT_CHANNEL    6
#define MOTOR_VOLTAGE_CHANNEL       3
#define SPARE_CHANNEL               2

#define CD4051_SETTLING_TIME_US     3

//==============================================================================
// SNES Gamepad

#define     GENERIC_GAMEPAD_VID         0x081F
#define     GENERIC_GAMEPAD_PID         0xE401

#define     GAMEPAD_DPAD_X_AXIS_LEFT    0x00
#define     GAMEPAD_DPAD_X_AXIS_NULL    0x7F
#define     GAMEPAD_DPAD_X_AXIS_RIGHT   0xFF

#define     GAMEPAD_DPAD_Y_AXIS_UP      0x00
#define     GAMEPAD_DPAD_Y_AXIS_NULL    0x7F
#define     GAMEPAD_DPAD_Y_AXIS_DOWN    0xFF

// codes for joystick mode 1 : DPAD control

#define DPAD_STOP       ((GAMEPAD_DPAD_X_AXIS_NULL << 8) + GAMEPAD_DPAD_Y_AXIS_NULL)

#define DPAD_FORWARD    ((GAMEPAD_DPAD_X_AXIS_NULL << 8) + GAMEPAD_DPAD_Y_AXIS_UP)
#define DPAD_BACKWARD   ((GAMEPAD_DPAD_X_AXIS_NULL << 8) + GAMEPAD_DPAD_Y_AXIS_DOWN)

#define DPAD_SPIN_RIGHT   ((GAMEPAD_DPAD_X_AXIS_RIGHT << 8) + GAMEPAD_DPAD_Y_AXIS_NULL)
#define DPAD_SPIN_LEFT   ((GAMEPAD_DPAD_X_AXIS_LEFT << 8) + GAMEPAD_DPAD_Y_AXIS_NULL)

#define ARC_FORWARD_RIGHT   ((GAMEPAD_DPAD_X_AXIS_RIGHT << 8) + GAMEPAD_DPAD_Y_AXIS_UP)
#define ARC_FORWARD_LEFT    ((GAMEPAD_DPAD_X_AXIS_LEFT << 8) + GAMEPAD_DPAD_Y_AXIS_UP)

#define ARC_BACKWARD_RIGHT   ((GAMEPAD_DPAD_X_AXIS_RIGHT << 8) + GAMEPAD_DPAD_Y_AXIS_DOWN)
#define ARC_BACKWARD_LEFT    ((GAMEPAD_DPAD_X_AXIS_LEFT << 8) + GAMEPAD_DPAD_Y_AXIS_DOWN)

enum  gamepad_state {DISABLED, ENABLED} ;
enum  gamepad_switch_state {released, pressed};
enum  gamepad_dpad_X_axis {X_AXIS_OFF, X_AXIS_LEFT, X_AXIS_RIGHT};
enum  gamepad_dpad_Y_axis {Y_AXIS_OFF, Y_AXIS_UP, Y_AXIS_DOWN};

//==============================================================================
// SSD1306 display

#define SSD1306_LCDWIDTH    128
#define SSD1306_LCDHEIGHT   64

#define SS1306_NOS_LCD_ROWS    4

#define NOS_FONTS           2

#define LCD_NOS_ROW_CHARACTERS     (SSD1306_LCDWIDTH/9)   // does not inclide '\0'

#define ICON_ROW            1

#define MESSAGE_ROW         2

#define SCROLL_ROW_UPPER    3
#define SCROLL_ROW_LOWER    4
#define SCROLL_AREA_NOS_ROWS   ((SCROLL_ROW_LOWER - SCROLL_ROW_UPPER) + 1)
#define STRING_COUNT(array_of_strings)   (sizeof(array_of_strings) / sizeof(array_of_strings[0]))

#define SCROLL_WINDOW       5

#define SSD1306_SPI_SPEED   8000000         // SSD1306 SPIMax=10MHz
#define SSD1306_I2C_SPEED   (400 * 1000)    // 400K bits/sec

// fonts

#define         TERMINAL_9x16_FONT_WIDTH                 9
#define         TERMINAL_9x16_FONT_HEIGHT               16

#define         ROBOKID_LCD_ICONS_FONT_15x16_WIDTH      15
#define         ROBOKID_LCD_ICONS_FONT_15x16_HEIGHT     16

// ICON  character codes

#define BLANK           32  // ' '
#define BATTERY_FULL    33  // '!'
#define BATTERY_75      34  // '"'
#define BATTERY_HALF    35  // '#'
#define BATTERY_25      36  // '$'
#define BATTERY_EMPTY   37  // '%'

#define DPAD_UP         38  // '&'
#define DPAD_DOWN       39  // '\''
#define DPAD_LEFT       40  // '('
#define DPAD_RIGHT      41  // ')'

#define SELECT_START    47  // '/'
#define SELECT          48  // '0'
#define START           49  // '1'

#define BUTTON_A_B      50  // '2'
#define BUTTON_A        51  // '3'
#define BUTTON_B        52  // '4'

#define BUTTON_X_Y      53  // '5'
#define BUTTON_X        54  // '6'
#define BUTTON_Y        55  // '7'

#define BUTTON_L_R      56  // '8'
#define BUTTON_L        57  // '9'
#define BUTTON_R        58  // ':'

#define GAMEPAD_CONNECTED   61  // '='
#define ERROR_ICON      63  // '?'

//==============================================================================
// sounder system

#define     SOUNDER_PIN_A           GP11

#define     SOUNDER_SLICE_CHANNEL   1

#define     SOUNDER_COUNT_FREQ      5000000    // 5MHz
#define     SOUNDER_PWM_CLK_DIV     (CPU_CLOCK_FREQUENCY / SOUNDER_COUNT_FREQ)  // init 25

#define     NOS_OCTAVE_NOTES        13
#define     MAX_TUNE_NOTES          32

//
// Frequencies of octave 4
//
#define     FREQ_C      262
#define     FREQ_CS     277
#define     FREQ_D      294
#define     FREQ_DS     311
#define     FREQ_E      330
#define     FREQ_F      349
#define     FREQ_FS     370
#define     FREQ_G      382
#define     FREQ_GS     415
#define     FREQ_A      440
#define     FREQ_AS     455
#define     FREQ_B      484

#define     NOTE_C      0
#define     NOTE_CS     1
#define     NOTE_D      2
#define     NOTE_DS     3
#define     NOTE_E      4
#define     NOTE_F      5
#define     NOTE_FS     6
#define     NOTE_G      7
#define     NOTE_GS     8
#define     NOTE_A      9
#define     NOTE_AS     10
#define     NOTE_B      11
#define     SILENT_NOTE 12

//==============================================================================
// Battery data
//
// Battery levels to be defined by experimentation.  

#define V_BATT_100_PERCENT  1743
#define V_BATT_75_PERCENT   1713
#define V_BATT_50_PERCENT   1676
#define V_BATT_25_PERCENT   1609

//==============================================================================
// System modes

typedef enum {
    PRIMARY_NULL_MODE,
    GAMEPAD_MODE, ACTIVITY_MODE, BUMP_MODE, FOLLOW_MODE, PROGRAM_MODE, 
    SKETCH_MODE, LAB_MODE, DISTANCE_MODE, TEST_MODE
} primary_sys_modes_te;

#define     NOS_PRIMARY_MODES   TEST_MODE
#define     FIRST_PRIMARY_MODE  GAMEPAD_MODE
#define     LAST_PRIMARY_MODE   TEST_MODE

typedef enum { 
    SECONDARY_NULL_MODE,
    GAMEPAD_MODE_0=(GAMEPAD_MODE*10), GAMEPAD_MODE_1, GAMEPAD_MODE_2,
    RUN_FORWARD=(ACTIVITY_MODE*10), RUN_BACKWARD, RUN_SPIN_RIGHT, RUN_SPIN_LEFT, RUN_TEST,  DEMO_MODE,
    LINE_BUMP_MODE=(BUMP_MODE*10), LINE_BUG_BUMP_MODE, WALL_BUMP_MODE,
    LINE_FOLLOW_MODE=(FOLLOW_MODE*10), LIGHT_FOLLOW_MODE,
    PROGRAM_MODE_0=(PROGRAM_MODE*10), PROGRAM_MODE_1, PROGRAM_MODE_2, PROGRAM_MODE_3, PROGRAM_MODE_4, PROGRAM_MODE_5,
    SKETCH_MODE_0=(SKETCH_MODE*10), SKETCH_MODE_1, SKETCH_MODE_2,
    LAB_MODE_0=(LAB_MODE*10), LAB_MODE_1, LAB_MODE_2,
    DISTANCE_MODE_0=(DISTANCE_MODE*10), DISTANCE_MODE_1, DISTANCE_MODE_2,
    TEST_MODE_0=(TEST_MODE*10),TEST_MODE_1, TEST_MODE_2, TEST_MODE_3, TEST_MODE_4,
} secondary_sys_modes_te;

#define   FIRST_GAMEPAD_MODE    GAMEPAD_MODE_0
#define   LAST_GAMEPAD_MODE     GAMEPAD_MODE_2

#define   FIRST_ACTIVITY_MODE   RUN_FORWARD
#define   LAST_ACTIVITY_MODE    RUN_TEST

#define   FIRST_BUMP_MODE       LINE_BUMP_MODE
#define   LAST_BUMP_MODE        WALL_BUMP_MODE

#define   FIRST_FOLLOW_MODE     LINE_FOLLOW_MODE
#define   LAST_FOLLOW_MODE      LIGHT_FOLLOW_MODE

#define   FIRST_PROGRAM_MODE    PROGRAM_MODE_0
#define   LAST_PROGRAM_MODE     PROGRAM_MODE_5

#define   FIRST_SKETCH_MODE     SKETCH_MODE_0
#define   LAST_SKETCH_MODE      SKETCH_MODE_2

#define   FIRST_DISTANCE_MODE   DISTANCE_MODE_0
#define   LAST_DISTANCE_MODE    DISTANCE_MODE_2

#define   FIRST_LAB_MODE        LAB_MODE_0
#define   LAST_LAB_MODE         LAB_MODE_2

#define   FIRST_TEST_MODE       TEST_MODE_0
#define   LAST_TEST_MODE        TEST_MODE_4

typedef enum {
    TERTIARY_NULL_MODE,
    level_3_0, level3_1,
} tertiary_sys_modes_te;

typedef enum {PLAY, COLLECT, SAVE, RECALL, DUMP} sequence_mode_te;


//==============================================================================
// Constants for system modes
//
// 1. gamepad mode 0
//
#define GAMEPAD_MODE_0_SLOW_SPEED  60
#define GAMEPAD_MODE_0_FAST_SPEED  80

//==============================================================================
// Freertos

typedef enum TASKS {
    TASK_ROBOKID, TASK_DRIVE_MOTORS, TASK_READ_SENSORS, TASK_DISPLAY,
    TASK_READ_GAMEPAD, TASK_SOUNDER, TASK_ERROR, TASK_SERIAL_OUTPUT, 
    TASK_LOG, TASK_BLINK
} task_t;

#define     NOS_TASKS   (TASK_BLINK + 1)

#define     TASK_READ_SENSORS_FREQUENCY                 50  // Hz
#define     TASK_READ_SENSORS_FREQUENCY_TICK_COUNT      ((1000/TASK_READ_SENSORS_FREQUENCY) * portTICK_PERIOD_MS)

#define     TASK_DISPLAY_LCD_FREQUENCY                  10  // Hz
#define     TASK_DISPLAY_TIME_UNIT                      (1000 / TASK_DISPLAY_LCD_FREQUENCY)
#define     TASK_DISPLAY_LCD_FREQUENCY_TICK_COUNT       ((1000/TASK_DISPLAY_LCD_FREQUENCY) * portTICK_PERIOD_MS)
#define     SCROLL_DELAY_MS                             2000
#define     SCROLL_DELAY_TICK_COUNT                     (SCROLL_DELAY_MS / TASK_DISPLAY_LCD_FREQUENCY_TICK_COUNT)    

#define     TASK_SOUNDER_FREQUENCY                      20  // Hz
#define     TASK_SOUNDER_TIME_UNIT                      (1000 / TASK_SOUNDER_FREQUENCY)
#define     TASK_SOUNDER_FREQUENCY_TICK_COUNT           ((1000 / TASK_SOUNDER_FREQUENCY) * portTICK_PERIOD_MS)

#define     TASK_LOG_FREQUENCY                          0.1  //HZ
#define     TASK_LOG_FREQUENCY_TICK_COUNT               (10000 * portTICK_PERIOD_MS)

#define     MOTOR_CMD_QUEUE_LENGTH          8
#define     ERROR_MESSAGE_QUEUE_LENGTH      8

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

#define     START_PULSE         gpio_put(LOG_PIN, 1)
#define     STOP_PULSE          gpio_put(LOG_PIN, 0)

#define     FOREVER     for(;;)
#define     HANG        for(;;)

#define     ATTRIBUTE_PACKED     __attribute__ ((__packed__))

#define     ENABLE_SCROLLER     LCD_scroll_data.enable = true
#define     DISABLE_SCROLLER    LCD_scroll_data.enable = false

#define     NOS_NOTES(note_array)   (sizeof(note_array)/sizeof(struct note_data_s))

#define WAIT_BUTTON_PRESSED(BUTTON)  xEventGroupWaitBits( eventgroup_push_buttons, (1 << BUTTON), false, false, portMAX_DELAY )
#define WAIT_BUTTON_RELEASED(BUTTON)  xEventGroupWaitBits( eventgroup_push_buttons, (1 << ((BUTTON) + 4)), false, false, portMAX_DELAY )

//==============================================================================
// definitions of system data structures
//==============================================================================
/**
 * @brief data structure for report returned from gamepad
 * 
 */
struct  SNES_gamepad_report_s   {  
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
} ;

//==============================================================================
/**
 * @brief data structure to hold current gamepad information
 * 
 */
struct gamepad_data_s  { 
    uint8_t     state;                  // ENABLED or DISABLED
    uint32_t    vid, pid;
    uint8_t     dpad_x, dpad_y;
    uint8_t     button_X, button_Y, button_A, button_B;
    uint8_t     button_L, button_R;
    uint8_t     button_START, button_SELECT;
} ;

struct system_status_s {
    error_codes_te     error_state;
} ;

struct __attribute__((__packed__)) motor_cmd_packet_s {
    motor_cmd_t     cmd;
    int8_t          param1, param2, param3;
} ;

struct motor_data_s {
    direction_t     motor_state;
    uint8_t         pwm_width;
    bool            flip;
} ;

struct  push_button_data_s {
    bool      switch_value;
    uint32_t  on_time;
};

struct vehicle_data_s {
    vehicle_state_t     vehicle_state;
    uint32_t            speed;
};

struct LED_data_s {
    uint8_t         pin_number;
    LED_state_te    state;
    bool            flash;
    uint8_t         flash_time;    // units of 20mS
    uint8_t         flash_counter;
    bool            flash_value;
} ;

struct line_sensor_data_s {
    uint8_t     percent_value;
    uint8_t     threshhold;
    bool        binary_value;
} ;

struct error_message_s {
    error_codes_te  error_code;
    task_t          task;
    uint64_t        log_time;
} ;

struct error_data_s {
    struct error_message_s     error_message_log[LOG_SIZE];
    uint8_t     error_log_ptr;
    uint16_t    error_count;
};

struct system_modes_s {
    primary_sys_modes_te    primary_mode;
    secondary_sys_modes_te  secondary_mode;
    tertiary_sys_modes_te   tertiary_mode;
};

//==============================================================================
// queue element for serial print facility
//
// Queue to print task  holds list of used indexes pointing to string to be printed
// Queus fro print task holsd list of indexes pointing to free string buffers

#define     NOS_PRINT_STRING_BUFFERS    8
#define     STRING_LENGTH        128

struct string_buffer_s {
    uint32_t    buffer_index;
};

//==============================================================================
//==============================================================================
// CD4051 analogue subsystem

#define     BUFF_SIZE               4
#define     A_D_GLITCH_THRESHOLD    2000
#define     GLITCH_COUNT_THRESHOLD  10

typedef enum {ANALOGUE_TYPE, DIGITAL_TYPE} channel_type_te;

struct circular_buffer_s {
    uint16_t    buffer[BUFF_SIZE];
    uint8_t     buff_ptr;
};

struct analogue_local_data_s {
    struct {
        struct circular_buffer_s    cir_buffer;
        uint16_t    last_value;
        uint8_t     sample_count;
    } raw;
    struct {
          uint8_t     dummy;
    } processed;
};

struct analogue_global_data_s {
    bool    active;
    bool    apply_filter;
    struct {
        uint16_t    current_value;
        uint8_t     percent_current_value;
        uint16_t    glitch_threshold;
        uint8_t     glitch_count;
        uint8_t     glitch_error_count_threshold;
        uint16_t    max_delta;
    } raw;
    struct {
        channel_type_te     channel_type;
        uint16_t            value;
        uint8_t             percent_value;
    } processed;
};

//==============================================================================
/**
 * @brief Task data
 */
struct task_data_s {
    TaskHandle_t            task_handle;
    uint8_t                 priority;
    StackType_t             *pxStackBase;
    configSTACK_DEPTH_TYPE  StackHighWaterMark;
    struct {
        uint32_t    last_exec_time;
        uint32_t    lowest_exec_time;
        uint32_t    highest_exec_time;
    };
};

//==============================================================================
//==============================================================================
/**
 * @brief   Central store of system data. Access by mutex - semaphore_system_IO_data
 */
struct system_IO_data_s  {
    struct system_modes_s               robokid_modes;
    struct system_status_s              system_status;      // error codes
    struct task_data_s                  task_data[NOS_TASKS];
    uint16_t                            system_voltage;
    struct motor_data_s                 motor_data[NOS_ROBOKID_MOTORS];
    struct LED_data_s                   LED_data[NOS_ROBOKID_LEDS];
    struct push_button_data_s           push_button_data[NOS_ROBOKID_PUSH_BUTTONS];
    struct analogue_global_data_s       analogue_global_data[NOS_CD4051_CHANNELS];
    struct line_sensor_data_s           line_sensor_data[NOS_ROBOKID_LINE_SENSORS];
    struct vehicle_data_s               vehicle_data;
} ;

//==============================================================================

#define     MAX_SCROLL_STRINGS             8
#define     MAX_SSD1306_STRING_LENGTH     15

struct LCD_scroll_data_s {
    bool        enable;
    uint8_t     nos_strings;
    uint8_t     string_count;
    uint8_t     nos_LCD_rows;
    uint8_t     first_LCD_row;
    uint8_t     scroll_delay;       // in units of LCD task (typ 100mS)
    uint8_t     scroll_delay_count;
    char        string_data[MAX_SCROLL_STRINGS][MAX_SSD1306_STRING_LENGTH];     // pointer  to list of strings
} ;

struct LCD_row_data_s {
    bool    dirty_bit;
    uint8_t font;
    char    row_string[16];
    bool    invert;
} ;

struct font_data_s {
    const char  *font;
    uint8_t     chars_per_row;
} ;

struct note_data_s {
    uint16_t     tone;
    uint16_t     duration_100mS;
};

struct  tune_data_s {
    bool                new;
    struct note_data_s  *note_pointer;      // (*note_pointer)[32] pointer to an array of structs
    uint16_t            nos_notes;
    bool                enable;
    bool                repeat_count;           // count repetitions
};


//==============================================================================
// Extern references
//==============================================================================
// System data structures

extern struct task_data_s task_data[];

extern struct gamepad_data_s   gamepad_data;
extern struct system_status_s  system_status;
extern struct system_IO_data_s system_IO_data;
extern struct tune_data_s      tune_data;

extern struct error_data_s      error_data;

extern struct font_data_s  font_data[];

extern const uint8_t *error_strings[];

// Hardware

extern const uint LED_PIN;
extern const uint LOG_PIN;
extern const uint BLINK_PIN;

// FreeRTOS components

extern void Task_Robokid(void *p);                      // tasks
extern void Task_read_sensors(void *p);
extern void Task_read_gamepad(void *p);
extern void Task_display_LCD(void *p);
extern void Task_drive_motors(void *p);
extern void Task_error(void *p);
extern void Task_sounder (void *p);
extern void Task_serial_output(void *p);
extern void Task_log_system_data(void *p);
extern void Task_blink_LED(void *p);

extern  TaskHandle_t taskhndl_Task_Robokid;
extern  TaskHandle_t taskhndl_Task_read_sensors;
extern  TaskHandle_t taskhndl_Task_read_gamepad;
extern  TaskHandle_t taskhndl_Task_display_LCD;
extern  TaskHandle_t taskhndl_Task_drive_motors;
extern  TaskHandle_t taskhndl_Task_error;
extern  TaskHandle_t taskhndl_Task_sounder;
extern  TaskHandle_t taskhndl_Task_serial_output;
extern  TaskHandle_t taskhndl_Task_blink_LED;

extern SemaphoreHandle_t semaphore_LCD_data;            //semaphores
extern SemaphoreHandle_t semaphore_SSD1306_display;
extern SemaphoreHandle_t semaphore_system_IO_data;
extern SemaphoreHandle_t semaphore_system_status;
extern SemaphoreHandle_t semaphore_gamepad_data;
extern SemaphoreHandle_t semaphore_tune_data;

extern QueueHandle_t queue_motor_cmds;                  // queues
extern QueueHandle_t queue_error_messages;
extern QueueHandle_t queue_print_string_buffers;
extern QueueHandle_t queue_free_buffers;

extern EventGroupHandle_t eventgroup_push_buttons;      // event groups

extern struct error_message_s     error_message_log[LOG_SIZE];

// SSD1306 LCD Fonts

extern const unsigned char Terminal_9x16[];
extern const unsigned char robokid_LCD_icons_font_15x16[];

extern const uint8_t *font_table[];
extern const unsigned char Terminal_12x16[];
extern const unsigned char Font_6x8[];
extern const unsigned char Segment_25x40[];
extern const unsigned char truck_bmp[1024];

extern struct LCD_row_data_s  LCD_row_data[];
extern struct LCD_scroll_data_s   LCD_scroll_data;

extern char print_string_buffers[NOS_PRINT_STRING_BUFFERS][STRING_LENGTH];

#endif /* __SYSTEM_H__ */