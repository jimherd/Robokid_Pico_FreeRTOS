## Robokid with Raspberry Pi Pico
----
Conversion of original Robokid system to use RP2040 Pico device

| Original | New|
|:--------:|:----------:|
| NXP MC9S08AW60 | Raspberry Pico RP2040 |
| 7-segment | SSD1306 LCD|
| MPC17511 H-bridge | DRV8833 H-bridge |
| Custom joystick | USB SNES gamepad |
| Custom timing code | FreeRTOS |
| Custom joystick code | TinyUSB |

### Tools

* Windows 10 laptop
* Visual Studio Code
* GCC 10.3.1 arm-none-eabi compiler
* CMake tools
* PicoProbe debug probe
* GLCD Font Creator
* USBlyzer
* PicoScope 2203

Jim Herd March 2022
