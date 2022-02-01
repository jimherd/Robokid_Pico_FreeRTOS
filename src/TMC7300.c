/**
 * @file    TMC7300.c
 * @author  Jim Herd 
 * @brief   Routines to control Trinamic TMC7300 H-bridge device
 * @version 0.1
 * @date    2021-10-31
 */

#include    <stdlib.h>

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
error_codes_t                   TMC7300_sys_error;      // global error flag

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

//----------------------------------------------------------------------------
/**
 * @brief Configure TMC7300 H-bridge chip
 * 
 */
void  TMC7300_Init(void) {

    TMC7300_sys_error = OK;       // clear global error variable
    
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

//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
/**
 * @brief  calculate 8-bit CRC on array of bytes
 * 
 * Uses a fast lookup table method to calculate 8-bit CRC.  Table inclues
 * precalculated values.
 * 
 * @param[in] data      array of 8-bit values
 * @param[in] length    number of array
 * @return              8-bit CRC value
 * 
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

//----------------------------------------------------------------------------
/**
 * @brief   Write to a TMC7300 register
 * 
 * Transmit 8 byte datagram to TMC7300 to write to a device register. No reply
 * will occur.
 * 
 * @param[in]   datagram    pointer to 8-byte array 
 * @return                  no value returned
 */
void TMC7300_write_reg(TMC7300_write_datagram_t *datagram) {

    uart_write_blocking(TMC7300_UART_PORT, (uint8_t *)datagram, sizeof(TMC7300_write_datagram_t));
}

//----------------------------------------------------------------------------
/**
 * @brief       Read a TMC7300 register    
 * 
 * Transmit 4 byte datagram to TMC7300 to read a device register and recieve 
 * a 12 byte reply.
 * 
 * @param[in]       datagram  pointer to 4-byte array
 * @return          error code
 */
error_codes_t  TMC7300_read_reg(TMC7300_read_datagram_t *datagram, TMC7300_read_reply_datagram_t *reply_datagram) {

    uart_write_blocking(TMC7300_UART_PORT, (uint8_t *)datagram, sizeof(TMC7300_read_datagram_t));
    uart_read_blocking(TMC7300_UART_PORT, (uint8_t *)&read_reply_datagram, sizeof(TMC7300_read_reply_datagram_t));
    uint8_t crc = TMC7300_CRC8((uint8_t *)&read_reply_datagram, LENGTH_READ_REPLY_DATAGRAM-1);
    if (crc != read_reply_datagram.crc) {
        return CRC_ERROR;
    }
    return OK;
}

//----------------------------------------------------------------------------
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
error_codes_t execute_cmd(command_t command, RW_mode_t RW_mode, uint32_t value) 
{
uint32_t tmp_value, target_register, *shadow_register;

    switch(command) {
        case SET_PWM_A : {
            
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
        case  ENABLE_MOTORS : {
            target_register = TMC7300_reg_data[CHOPCONF_IDX].register_address;
            shadow_register = &TMC7300_reg_data[CHOPCONF_IDX].shadow_value;
            tmp_value = 1;
            switch (RW_mode) {
                case WRITE_CMD : {
                    MERGE_REG_VALUE(*shadow_register, tmp_value, TMC7300_ENABLEDRV_MASK, TMC7300_ENABLEDRV_SHIFT);
                    break;
                }
            }
        }
        case  DISABLE_MOTORS : {
            target_register = TMC7300_reg_data[CHOPCONF_IDX].register_address;
            shadow_register = &TMC7300_reg_data[CHOPCONF_IDX].shadow_value;
            tmp_value = 0;
            switch (RW_mode) {
                case WRITE_CMD : {
                    MERGE_REG_VALUE(*shadow_register, tmp_value, TMC7300_ENABLEDRV_MASK, TMC7300_ENABLEDRV_SHIFT);
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

//----------------------------------------------------------------------------
/**
 * @brief       set delay between recieving a read datagram and sending a reply
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

void set_break_mode(brake_mode_t mode)
{
uint32_t  tmp_value, *shadow_register;

    switch (mode) {
        FREEWHEEL:
                shadow_register = &TMC7300_reg_data[TMC7300_CURRENT_LIMIT].shadow_value;
                MERGE_REG_VALUE(*shadow_register, tmp_value, TMC7300_FREEWHEEL_MODE_OP, TMC7300_MOTORRUN_SHIFT);
                break;
        BRAKE_LS_DRIVER:
                break;
        BRAKE_HS_DRIVER:
                break;
    }
}

//----------------------------------------------------------------------------
/**
 * @brief Set the motor object
 * @note                The motor speed is specified in the range of 0 to 100% 
 *                      which is converted to the range 0 to 255
 * @param[in] unit      LEFT_MOTOR or RIGHT_MOTOR
 * @param[in] state     MOTOR_OFF, MOTOR_FORWARD, MOTOR_BACKWARD
 * @param[in] pwm_width 0% to 100%
 * 
 * @return    error_code_t  error code
 */
error_codes_t  set_motor(motor_t unit, uint8_t pwm_width) 
{
motor_t  left_motor_state, right_motor_state;

    
   // uint32_t pulse_count = (period_count/100) * (100 - pwm_width);
    if (unit >= NOS_ROBOKID_MOTORS) {
        return BAD_MOTOR_NUMBER;
    }
    if (abs(pwm_width > 100)) {
        return BAD_PWM_PERCENT;
    }

    if (unit == LEFT_MOTOR) {
        
    }
    
    if (unit == RIGHT_MOTOR) {
        // switch (state) {
        //     case MOTOR_OFF :        // set FREEWHEEL condition
                
        //         left_motor_state = MOTOR_OFF;
        //         break;
        //     case MOTOR_FORWARD :    // set LOW on RM_PWM2 and pwm on LM_PWM1
                
        //         left_motor_state = MOTOR_FORWARD;
        //         break;
        //     case MOTOR_BACKWARD :   // set LOW on RM_PWM1 and RM_PWM2
                
        //         left_motor_state = MOTOR_BACKWARD;
        //         break;
        //     case MOTOR_BRAKE :      // set BRAKE condition
                
        //         left_motor_state = MOTOR_BRAKE;
        //         break;
        // }
    }
//    set_vehicle_state(); 
}

//----------------------------------------------------------------------------
/**
 * @brief Set the motor config object
 * 
 * @param   motor_config_data 
 * @return  error_codes_t   OK, or fault code (-ve value)
 */
error_codes_t  set_motor_config(motor_config_data_t  *motor_config_data) 
{

    return OK;
}

//----------------------------------------------------------------------------
/**
 * @brief   Enable TMC7300 H-bridge drivers
 * 
 * @return  error_codes_t   OK, or fault code (-ve value)
 */
error_codes_t  enable_motors(void)
{
    return OK;
} 

//----------------------------------------------------------------------------
/**
 * @brief   Disable TMC7300 H-bridge drivers
 * 
 * @return  error_codes_t   OK, or fault code (-ve value)
 */
error_codes_t  disable_motors(void)
{
    return OK;
} 
