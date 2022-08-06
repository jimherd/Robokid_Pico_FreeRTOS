/**
 * @file    error_codes.h
 * @author  Jim Herd 
 * @brief   System wide error codes
 * @date    2022-03-31
 */
#ifndef __ERROR_CODES_H__
#define __ERROR_CODES_H__

//==============================================================================
// list of errors

typedef enum  {
    OK                              =  0,
    FAULT                           = -1,
    BAD_PWM_PERCENT_WIDTH           = -2,
    BAD_MOTOR_NUMBER                = -3,
    BAD_MOTOR_COMMAND               = -4,
    USB_CONTROLLER_NOT_CONNECTED    = -5,
    GAMEPAD_ERROR_READING_VID_PID   = -6,
    GLITCH_ERRORS_ON_AD_READ        = -7,
} error_codes_te;

//==============================================================================
// size of circular buffer holding logged errors

#define     LOG_SIZE    50

#endif