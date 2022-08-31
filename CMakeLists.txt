cmake_minimum_required(VERSION 3.12)

file(TO_CMAKE_PATH "$ENV{PICO_SDK_PATH}" PICO_SDK_PATH_CMAKE)

# initialize the SDK based on PICO_SDK_PATH
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(Robokid_Pico_FreeRTOS C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initialize the Pico SDK
pico_sdk_init()

# add gcc compiler options

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fshort-enums -Wunused-result ")

# collect all source .c files
FILE(GLOB Robokid_Pico_Freetros_src ${CMAKE_SOURCE_DIR}/src/*.c)

add_executable(${PROJECT_NAME}
    ${Robokid_Pico_Freetros_src}
)

FILE(GLOB FreeRTOS_src ${CMAKE_SOURCE_DIR}/lib/FreeRTOS-Kernel/*.c)

add_library( FreeRTOS STATIC
    ${FreeRTOS_src}
    ${CMAKE_SOURCE_DIR}/lib/FreeRTOS-Kernel/portable/GCC/ARM_CM0/port.c
    ${CMAKE_SOURCE_DIR}/lib/FreeRTOS-Kernel/portable/MemMang/heap_1.c    # heap_4.c
)

target_include_directories( FreeRTOS PUBLIC
    ${CMAKE_SOURCE_DIR}/lib/FreeRTOS-Kernel/include
    ${CMAKE_SOURCE_DIR}/lib/FreeRTOS-Kernel/portable/GCC/ARM_CM0
    ${CMAKE_SOURCE_DIR}/include
)

target_include_directories ( ${PROJECT_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/icons
    ${CMAKE_SOURCE_DIR}/lib/tinyusb/src
)

target_link_libraries( ${PROJECT_NAME} 
    pico_stdlib 
    pico_multicore
    hardware_gpio 
    hardware_i2c
    hardware_spi
    hardware_pwm
    hardware_adc
    FreeRTOS
    tinyusb_board 
    tinyusb_host
    )

pico_add_extra_outputs(${PROJECT_NAME})

# Enable usb output, disable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)

# print executable size
add_custom_command(TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND date.exe /t >> ${CMAKE_SOURCE_DIR}/File_size.txt
    COMMAND arm-none-eabi-size.exe -G Robokid_Pico_FreeRTOS.elf
    COMMAND arm-none-eabi-size.exe -G Robokid_Pico_FreeRTOS.elf  >> ${CMAKE_SOURCE_DIR}/File_size.txt
    VERBATIM
)