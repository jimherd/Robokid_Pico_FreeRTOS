/**
 * @file    neopixel.c
 * @author  Jim Herd
 * @brief   neopixel routines
 */

#include "system.h"
#include "common.h"

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

#include "neopixel.h"

#include "Pico_IO.h"

void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return
            ((uint32_t) (r) << 8) |
            ((uint32_t) (g) << 16) |
            (uint32_t) (b);
}
