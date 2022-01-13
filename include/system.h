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

#define     TMC7300_UART_PORT   uart1  
#define     BAUD_RATE           115200   

#define     GENERIC_GAMEPAD_VID     0x081F
#define     GENERIC_GAMEPAD_PID     0xE401
                                                                             
#define     UART_TX_PIN         GP8          // Pin 11
#define     UART_RX_PIN         GP9          // Pin 12
#define     TMC7300_EN_PIN      GP10         // Pin 14

#define     NOS_ROBOKID_MOTORS  2

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

//==============================================================================
// Macros
//==============================================================================

#define     DISABLE_POWER_STAGE          gpio_put(TMC7300_EN_PIN, 0)
#define     ENABLE_POWER_STAGE           gpio_put(TMC7300_EN_PIN, 1)

#define     FOREVER     for(;;)
#define     HANG        for(;;)

//struct 

//==============================================================================
// Extern references
//==============================================================================

extern const uint LED_PIN;
extern SemaphoreHandle_t semaphore_gamepad_data;

extern void Task_read_gamepad(void *p);
extern void Task_blink_LED(void *p);
extern void Task_display_LCD(void *p);
extern void Task_drive_motors(void *p);

extern const unsigned char Terminal12x16[];
extern const unsigned char Font_6x8[];
extern const unsigned char Segment_25x40[];
extern const unsigned char truck_bmp[1024];
// extern const unsigned char icon1[128];
// extern const unsigned char icon2[128];
// extern const unsigned char icon3[128];
// extern const unsigned char icon4[128];


typedef struct {
    uint8_t         page;
    uint8_t         segment;
    unsigned char   *buffer;
} icon_data_t;


#endif /* _SYSTEM_H_ */