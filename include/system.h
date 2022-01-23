/**
 * @file system.h
 * @author Jim Herd 
 * @brief  Constants relating to Robokid robot
 * @version 0.1
 * @date 2022-01-09
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include    "FreeRTOS.h"
#include    "semphr.h"      // from FreeRTOS

#ifndef _SYSTEM_H_
#define _SYSTEM_H_

//==============================================================================
// Constants
//==============================================================================

// TMC7300 H-bridge

#define     TMC7300_UART_PORT   uart1  
#define     BAUD_RATE           115200   

#define     UART_TX_PIN         GP8          // Pin 11
#define     UART_RX_PIN         GP9          // Pin 12
#define     TMC7300_EN_PIN      GP10         // Pin 14

#define     NOS_ROBOKID_MOTORS  2

#define     SPI_SPEED_5MHz      5000000

// SNES Gamepad

#define     GENERIC_GAMEPAD_VID     0x081F
#define     GENERIC_GAMEPAD_PID     0xE401

#define     GAMEPAD_DPAD_X_AXIS_LEFT    0x00
#define     GAMEPAD_DPAD_X_AXIS_NULL    0x7F
#define     GAMEPAD_DPAD_X_AXIS_RIGHT   0xFF

#define     GAMEPAD_DPAD_Y_AXIS_UP      0x00
#define     GAMEPAD_DPAD_Y_AXIS_NULL    0x7F
#define     GAMEPAD_DPAD_Y_AXIS_DOWN    0xFF

// SSD1306 display

#define SSD1306_LCDWIDTH 128
#define SSD1306_LCDHEIGHT 64

//==============================================================================
// enum definitions
//==============================================================================

typedef enum  { GCONF_IDX,
                GSTAT_IDX,
                IFCNT_IDX,
                SLAVECONF_IDX,
                IOIN_IDX,
                CURRENT_LIMIT_IDX,
                PWM_AB_IDX,
                CHOPCONF_IDX,
                DRV_STATUS_IDX,
                PWMCONF_IDX
} TMC7300_reg_index_t;

enum  gamepad_state {DISABLED, ENABLED} ;
enum  gamepad_switch_state {released, pressed};
enum  gamepad_dpad_X_axis {X_AXIS_OFF, X_AXIS_LEFT, X_AXIS_RIGHT};
enum  gamepad_dpad_Y_axis {Y_AXIS_OFF, Y_AXIS_UP, Y_AXIS_DOWN};


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

//==============================================================================
// definitions of system data structures
//==============================================================================

/**
 * @brief data structure for report returned from gamepad
 * 
 */
typedef struct TU_ATTR_PACKED  {    // packed attribute
    uint8_t     dpad_x, dpad_y;         // dpad returned at 8-bit values
    uint8_t     dummy2, dummy3,dummy4;

    struct {
        uint8_t dummy5      :4;
        uint8_t BUTTON_X    :1;
        uint8_t BUTTON_A    :1;
        uint8_t BUTTON_B    :1;
        uint8_t BUTTON_Y    :1;
    };

    struct {
        uint8_t BUTTON_L      :1;
        uint8_t BUTTON_R      :1;
        uint8_t dummy6        :2;
        uint8_t BUTTON_START  :1;
        uint8_t BUTTON_SELECT :1;
        uint8_t dummy7        :2;
    };
    uint8_t     dummy8;
} SNES_gamepad_report_t;

typedef struct {
    uint8_t     state;                  // ENABLED or DISAPLED
    uint8_t     vid, pid;
    uint8_t     dpad_x, dpad_y;
    uint8_t     button_X, button_Y, button_A, button_B;
    uint8_t     button_L, button_R;
    uint8_t     BUTTON_START, BUTTON_SELECT;
} gamepad_data_t;

typedef struct {
    uint8_t         page;
    uint8_t         segment;
    unsigned char   *buffer;
} icon_data_t;

typedef struct {
    uint8_t     error_state;
    uint8_t     dpad_state;
} system_status_t;

//==============================================================================
// Extern references
//==============================================================================

// System data structures

extern SNES_gamepad_report_t SNES_gamepad_report;
extern gamepad_data_t gamepad_data;
extern system_status_t system_status;

// Hardware

extern const uint LED_PIN;
extern const uint LOG_PIN;

// FreeRTOS components

extern SemaphoreHandle_t semaphore_system_status;
extern SemaphoreHandle_t semaphore_gamepad_data;

extern void Task_read_gamepad(void *p);
extern void Task_blink_LED(void *p);
extern void Task_display_LCD(void *p);
extern void Task_drive_motors(void *p);

// SSD1306 LCD Fonts

extern const uint8_t *font_table[];
extern const unsigned char Terminal12x16[];
extern const unsigned char Font_6x8[];
extern const unsigned char Segment_25x40[];
extern const unsigned char truck_bmp[1024];
extern const unsigned char robokid_LCD_icons_font15x16[];
extern const unsigned char Terminal9x16[];

//==============================================================================
// system definitions
//==============================================================================




#endif /* _SYSTEM_H_ */