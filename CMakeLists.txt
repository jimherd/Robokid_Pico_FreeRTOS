cmake_minimum_required(VERSION 3.12)

file(TO_CMAKE_PATH "$ENV{PICO_SDK_PATH}" PICO_SDK_PATH_CMAKE)

# initialize the SDK based on PICO_SDK_PATH
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(Robokid_pico_RTOS C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initialize the Pico SDK
pico_sdk_init()


add_executable(${PROJECT_NAME}
    src/main.c
)

FILE(GLOB FreeRTOS_src FreeRTOS-Kernel/*.c)

add_library( 
    FreeRTOS STATIC
    ${FreeRTOS_src}
    FreeRTOS-Kernel/portable/GCC/ARM_CM0/port.c
    FreeRTOS-Kernel/portable/MemMang/heap_4.c
)

target_include_directories( FreeRTOS PUBLIC
        FreeRTOS-Kernel/include
        include/
        FreeRTOS-Kernel/portable/GCC/ARM_CM0
    )

target_link_libraries( ${PROJECT_NAME} 
    pico_stdlib 
    hardware_gpio 
    hardware_spi
    FreeRTOS
    )

    
pico_add_extra_outputs(${PROJECT_NAME})

# Enable usb output, disable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)