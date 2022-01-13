/**
 * @file    TMC7300.c
 * @author  Jim Herd 
 * @brief   Routines to control Trinamic TMC7300 H-bridge device
 * @version 0.1
 * @date    2021-10-31
 */
#include <stdlib.h>

#include "TMC7300.h"
#include "TMC7300_Registers.h"
#include "TMC7300_Fields.h"
#include "pico/stdlib.h"
#include "hardware/uart.h"

#include "system.h"

//==============================================================================
// Global variables and structures
//==============================================================================

TMC7300_write_datagram_t        write_datagram;
TMC7300_read_datagram_t         read_datagram;
TMC7300_read_reply_datagram_t   read_reply_datagram;
TMC7300_errors_t                TMC7300_sys_error;      // global error flag

//
// Structure to hold data relevant to the 10 registers in the TMC7300 device

register_data_t TMC7300_reg_data[TMC7300_NOS_registers] = {
//    register            access mode      init value    shadow value
    {TMC7300_GCONF,         READ_WRITE,    0x00000000,    0x00000000 },
    {TMC7300_GSTAT,         READ_WRITE,    0x00000000,    0x00000000 },
    {TMC7300_IFCNT,         READ_ONLY,     0x00000000,    0x00000000 },
    {TMC7300_SLAVECONF,     WRITE_ONLY,    0x00000000,    0x00000000 },
    {TMC7300_IOIN,          READ_ONLY,     0x00000000,    0x00000000 },
    {TMC7300_CURRENT_LIMIT, WRITE_ONLY,    0x00001F01,    0x00001F01 },
    {TMC7300_PWM_AB,        WRITE_ONLY,    0x00000000,    0x00000000 },
    {TMC7300_CHOPCONF,      READ_WRITE,    0x13008001,    0x13008001 },
    {TMC7300_DRV_STATUS,    READ_ONLY,     0x00000000,    0x00000000 },
    {TMC7300_PWMCONF,       READ_WRITE,    0xC40D1024,    0xC40D1024 },
};

//==============================================================================
// Functions
//==============================================================================
// TMC7300_Init
//==============================================================================
/**
 * @brief Configure TMC7300 H-bridge chip
 * 
 * @param       none
 * @return      void
 * 
 */

void  TMC7300_Init(void) {

    TMC7300_sys_error = NO_ERROR;       // clear global error variable
    
    uart_init(TMC7300_UART_PORT, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    gpio_init(TMC7300_EN_PIN);
    gpio_set_dir(TMC7300_EN_PIN, GPIO_OUT);
    gpio_pull_down(TMC7300_EN_PIN);         // should default but just make sure
    DISABLE_POWER_STAGE;                    // essential before chip configuration

    reset_TMC7300();

    for (int i=0 ; i < 1000 ; i++) {
        set_master_slave_delay(DEFAULT_DELAY_TO_SEND_US);
    }
}

//==============================================================================
// Initialise TMC7300 registers and shadow registers
//==============================================================================
/**
 * @brief Initialise TMC7300 registers and shadow registers
 * 
 */

void reset_TMC7300(void) {

    for (uint32_t i = 0; i < TMC7300_NOS_registers ; i++) {
        if ((TMC7300_reg_data[i].RW_mode == READ_WRITE) || 
                        (TMC7300_reg_data[i].RW_mode == WRITE_ONLY)) {
            create_write_datagram(&write_datagram, TMC7300_reg_data[i].register_address, TMC7300_reg_data[i].init_value);
            TMC7300_write_reg(&write_datagram);
        } else {
            create_read_datagram(&read_datagram, TMC7300_reg_data[i].register_address);
            TMC7300_reg_data[i].shadow_value = TMC7300_read_reg(&read_datagram, &read_reply_datagram);
        }
    }
}


//==============================================================================
// create_write_datagram
//==============================================================================
/**
 * @brief Create an 8-byte write datagram for TMC7300
 * 
 * @param[out]  datagram            pointer to an 8-byte TMC7300 write command struct
 * @param[in]   register_address    target TMC7300 register for command
 * @param[in]   register_value      value to be written to register
 * 
 * @return      void
 */

void create_write_datagram(TMC7300_write_datagram_t *datagram, uint8_t register_address, uint32_t register_value) {

    datagram->sync_byte        =  TMC7300_SYNC_BYTE;
    datagram->slave_address    = 0;
    datagram->register_address = register_address | TMC7300_WRITE_BIT;
    datagram->data             = register_value;
    datagram->crc              = TMC7300_CRC8((uint8_t *)datagram, LENGTH_WRITE_DATAGRAM);

}

//==============================================================================
// create_read_datagram
//==============================================================================
/**
 * @brief Create an 8-byte write datagram for TMC7300
 * 
 * @param[out]  datagram            pointer to 4-byte TMC7300 read command struct
 * @param[in]   register_address    target TMC7300 register for command 
 * 
 * @return      void
 */
void create_read_datagram(TMC7300_read_datagram_t *datagram, uint8_t register_address){

    datagram->sync_byte        =  TMC7300_SYNC_BYTE;
    datagram->slave_address    = 0;
    datagram->register_address = register_address | TMC7300_READ_BIT;
    datagram->crc              = TMC7300_CRC8((uint8_t *)datagram, LENGTH_WRITE_DATAGRAM-1);
}

//==============================================================================
// TMC7300_CRC8 
//==============================================================================
/**
 * @brief  calculate 8-bit CRC on array of bytes
 * 
 * @param[in] data      array of 8-bit values
 * @param[in] length    number of array
 * @return              8-bit CRC value
 * 
 * @note    Fast CRC generation using table lookup
 * @author  Trinamic
 * @date    Oct 2021
 */
uint8_t TMC7300_CRC8(uint8_t *data, uint32_t length) {

uint8_t crc = 0;

    while(--length) {
        crc = TMC7300_crc_table[crc ^ *data++];
    }
    return crc;
}

//==============================================================================
// TMC7300_write_reg
//==============================================================================
/**
 * @brief Transmit 8 byte datagram to TMC7300 to write to a device register
 * 
 * @param   datagram    pointer to 8-byte array 
 * @return              no value returned
 */
void TMC7300_write_reg(TMC7300_write_datagram_t *datagram) {

    uart_write_blocking(TMC7300_UART_PORT, (uint8_t *)datagram, sizeof(TMC7300_write_datagram_t));
}

//==============================================================================
// TMC7300_read_reg
//==============================================================================
/**
 * @brief Transmit 4 byte datagram to TMC7300 to read a device register and
 *        recieve a 12 byte reply
 * 
 * @param datagram  pointer to 4-byte array
 * @return          error code
 */
TMC7300_errors_t  TMC7300_read_reg(TMC7300_read_datagram_t *datagram, TMC7300_read_reply_datagram_t *reply_datagram) {

    uart_write_blocking(TMC7300_UART_PORT, (uint8_t *)datagram, sizeof(TMC7300_read_datagram_t));
    uart_read_blocking(TMC7300_UART_PORT, (uint8_t *)&read_reply_datagram, sizeof(TMC7300_read_reply_datagram_t));
    uint8_t crc = TMC7300_CRC8((uint8_t *)&read_reply_datagram, LENGTH_READ_REPLY_DATAGRAM-1);
    if (crc != read_reply_datagram.crc) {
        return CRC_ERROR;
    }
    return NO_ERROR;
}

//==============================================================================
// TMC7300_command
//==============================================================================
/**
 *
 * @brief execute the set of TMC7300 commands
 * 
 * @note    A read command will load the value into the shadow register for later access
 * 
 * @param[in]   command     enum list of commands
 * @param[in]   RW_mode     READ_CMD of WRITE_CMD
 * @param[in]   value       32-bit value to be written by WRITE_CMD
 * 
 * @return                  error code ; OK if no errors detected
 * 
 */
TMC7300_errors_t execute_cmd(command_t command, RW_mode_t RW_mode, uint32_t value) {

uint32_t tmp_value, target_register, *shadow_register;

    switch(command) {
        case SET_PWM_A : {
            if (abs(value > 100)) {     // check PWM % value is legal
                return BAD_PWM_PERCENT;
            }
            target_register = TMC7300_reg_data[PWM_AB_IDX].register_address;
            shadow_register = &TMC7300_reg_data[PWM_AB_IDX].shadow_value; 
            switch (RW_mode) {
                case READ_CMD : {
                }
                case WRITE_CMD : {
                    tmp_value = (value * 255) * 100;   // scale value to +/-100%
                    MERGE_REG_VALUE(*shadow_register, tmp_value, TMC7300_PWM_A_MASK, TMC7300_PWM_A_SHIFT);
                    break;
                }
            }
            break;
        }
        case SET_PWM_B : {
            if (abs(value > 100)) {
                return BAD_PWM_PERCENT;
            }
            target_register = TMC7300_reg_data[PWM_AB_IDX].register_address;
            shadow_register = &TMC7300_reg_data[PWM_AB_IDX].shadow_value;
            switch (RW_mode) {
                case READ_CMD : {
                }
                case WRITE_CMD : {
                    tmp_value = (value * 255) * 100;    // scale value to +/-100%
                    MERGE_REG_VALUE(*shadow_register, tmp_value, TMC7300_PWM_B_MASK, TMC7300_PWM_B_SHIFT);
                    break;
                }
            }
            break;
        }
        case  SET_SEND_DELAY : {
            tmp_value = value;
            target_register = TMC7300_reg_data[SLAVECONF_IDX].register_address;
            shadow_register = &TMC7300_reg_data[SLAVECONF_IDX].shadow_value;
            switch (RW_mode) {
                case WRITE_CMD : {
                    MERGE_REG_VALUE(*shadow_register, tmp_value, TMC7300_SLAVECONF_MASK, TMC7300_SLAVECONF_SHIFT);
                    break;
                }
            }
        }
    //
    // format datagram, send to TMC7300, and update shadow register
    
        switch (RW_mode) {
                case READ_CMD : {
                    create_read_datagram(&read_datagram, target_register);
                    TMC7300_read_reg(&read_datagram, &read_reply_datagram);
                    *shadow_register = read_reply_datagram.data;
                    break;
                }
                case WRITE_CMD : {
                    create_write_datagram(&write_datagram, tmp_value, target_register);
                    TMC7300_write_reg(&write_datagram);
                    *shadow_register = write_datagram.data;
                    break;
                }
        }     // end execute switch
    }   // end command switch
}

//==============================================================================
// set_master_slave_delay
//==============================================================================
/**
 * @brief 
 * 
 *
 * @brief set delay between recieving a read datagram and sending a reply
 * 
 * @param[in]   delay in units of UART bit times 
 * @return      no value returned
 * 
 */
void set_master_slave_delay(uint32_t bit_times) {

uint32_t  tmp_bit_times, master_slave_delay;

    tmp_bit_times = bit_times;
    if (bit_times > BIT_TIMES_MAX) {
        tmp_bit_times = BIT_TIMES_MAX;
    }
    tmp_bit_times = tmp_bit_times & 0xFFFFFFF8;

    master_slave_delay = (((1000000 * tmp_bit_times) / BAUD_RATE) << TMC7300_SLAVECONF_SHIFT);

    create_write_datagram(&write_datagram, TMC7300_SLAVECONF, (uint8_t)master_slave_delay);
    TMC7300_write_reg(&write_datagram);
}


// /* *
//  * @brief compute absolute valus of a 32 bit integer
//  * 
//  * @param value 
//  * @return int32_t 
//  */
// int32_t abs_int32(int32_t value) {
//     return ((value < 0) ? -value : value);
// }
