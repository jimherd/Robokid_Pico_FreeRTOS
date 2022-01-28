/**
 * @file error.h
 * @author Jim Herd
 * @brief Hold data for error announcement system
 * @version 0.1
 * @date 2022-01-27
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef __ERROR_H__
#define __ERROR_H__

#include <stdio.h>

#define     OK      0

const uint8_t *error_strings[] = {
    "System OK",
    "Fault",
};

#endif