/**
 * @file    neopixel.h
 * @author  Jim Herd
 * @brief   neopixel data
 * @date    2022-02-26
 */
#ifndef __NEOPIXEL_H__
#define __NEOPIXEL_H__

#include <stdio.h>
#include "system.h"

void put_pixel(uint32_t pixel_grb);
uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b);

#endif