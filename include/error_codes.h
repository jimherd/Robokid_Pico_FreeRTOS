/**
 * @file    error_codes.h
 * @author  Jim Herd 
 * @brief   System wide error codes
 * @date    2022-03-31
 */
#ifndef __ERROR_CODES_H__
#define __ERROR_CODES_H__

//==============================================================================
// errors

typedef enum {
    OK                      =  0,
    FAULT                   = -1,
    BAD_PWM_PERCENT_WIDTH   = -2,
    BAD_MOTOR_NUMBER        = -3,
    BAD_MOTOR_COMMAND       = -4,
} error_codes_e;

#endif