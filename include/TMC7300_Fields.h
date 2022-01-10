/**
 * @file TMC7300_Fields.h
 * @author Trinamic. Additions by Jim Herd
 * @brief Mask and shift constants for fields in TMC7300 registers
 * @version 0.1
 * @date 2021-11-23
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef TMC7300_FIELDS
#define TMC7300_FIELDS

// Register GCONF
#define TMC7300_PWM_DIRECT_MASK    0x00000001
#define TMC7300_PWM_DIRECT_SHIFT   0
    #define TMC7300_NORMAL_MODE         0x01
#define TMC7300_EXTCAP_MASK        0x00000002
#define TMC7300_EXTCAP_SHIFT       1
    #define TMC7300_INTERNAL_CAP        0x00    //default
    #define TMC7300_EXTERNAL_CAP        0x01
#define TMC7300_PAR_MODE_MASK      0x00000004
#define TMC7300_PAR_MODE_SHIFT     2
    #define TMC7300_DUAL_MOTOR_MODE     0x00
    #define TMC7300_SINGLE_MOTOR_MODE   0x01    // default
#define TMC7300_TEST_MODE_MASK     0x00000080
#define TMC7300_TEST_MODE_SHIFT    7

// Register GSTAT
#define TMC7300_RESET_MASK         0x00000001
#define TMC7300_RESET_SHIFT        0
    #define TMC7300_RESET_STAT_FLAG     0x01
#define TMC7300_DRV_ERR_MASK       0x00000002
#define TMC7300_DRV_ERR_SHIFT      1
    #define TMC7300_DRV_ERR_STAT_FLAG   0x02
#define TMC7300_U3V5_MASK          0x00000004
#define TMC7300_U3V5_SHIFT         2
    #define TMC7300_u3v5_STAT_FLAG      0x04

// Register IFCNT
#define TMC7300_IFCNT_MASK         0x000000FF
#define TMC7300_IFCNT_SHIFT        0

// Register SLAVECONF
#define TMC7300_SLAVECONF_MASK     0x00000F00
#define TMC7300_SLAVECONF_SHIFT    8

// Register IOIN
#define TMC7300_EN_MASK            0x00000001
#define TMC7300_EN_SHIFT           0
#define TMC7300_NSTDBY_MASK        0x00000002
#define TMC7300_NSTDBY_SHIFT       1
#define TMC7300_AD0_MASK           0x00000004
#define TMC7300_AD0_SHIFT          2
#define TMC7300_AD1_MASK           0x00000008
#define TMC7300_AD1_SHIFT          3
#define TMC7300_DIAG_MASK          0x00000010
#define TMC7300_DIAG_SHIFT         4
#define TMC7300_UART_ENABLED_MASK  0x00000020
#define TMC7300_UART_ENABLED_SHIFT 5
#define TMC7300_UART_INPUT_MASK    0x00000040
#define TMC7300_UART_INPUT_SHIFT   6
#define TMC7300_MODE_INPUT_MASK    0x00000080
#define TMC7300_MODE_INPUT_SHIFT   7
#define TMC7300_A2_MASK            0x00000100
#define TMC7300_A2_SHIFT           8
#define TMC7300_A1_MASK            0x00000200
#define TMC7300_A1_SHIFT           9
#define TMC7300_COMP_A1A2_MASK     0x00000400
#define TMC7300_COMP_A1A2_SHIFT    10
#define TMC7300_COMP_B1B2_MASK     0x00000800
#define TMC7300_COMP_B1B2_SHIFT    11
#define TMC7300_VERSION_MASK       0xFF000000
#define TMC7300_VERSION_SHIFT      24

// Register CURRENT_LIMIT
#define TMC7300_MOTORRUN_MASK      0x00000001
#define TMC7300_MOTORRUN_SHIFT     0
    #define TMC7300_NORMAL_MODE_OP      0x00
    #define TMC7300_FREEWHEEL_MODE_OP   0x01    // default
#define TMC7300_IRUN_MASK          0x00001F00
#define TMC7300_IRUN_SHIFT         8
    #define TMC7300_IRUN_MAX            0x1F    //default

// Register PWM_AB
#define TMC7300_PWM_A_MASK         0x000001FF
#define TMC7300_PWM_A_SHIFT        0
#define TMC7300_PWM_B_MASK         0x01FF0000
#define TMC7300_PWM_B_SHIFT        16
    #define TMC7300_PWM_MAX_PLUS        0x0FF   // +100% : +255
    #define TMC7300_PWM_ZERO            0x000   //    0% :    0
    #define TMC7300_PWM_MAX_MINUS       0x101   // -100% : -255

//Register CHOPCONF
#define TMC7300_ENABLEDRV_MASK     0x00000001
#define TMC7300_ENABLEDRV_SHIFT    0
#define TMC7300_TBL_MASK           0x00018000
#define TMC7300_TBL_SHIFT          15
#define TMC7300_DISS2G_MASK        0x40000000
#define TMC7300_DISS2G_SHIFT       30
#define TMC7300_DISS2VS_MASK       0x80000000
#define TMC7300_DISS2VS_SHIFT      31

// Register PWMCONF
#define TMC7300_PWM_FREQ_MASK      0x00030000
#define TMC7300_PWM_FREQ_SHIFT     16
    #define TMC7300_PWM_FREQ_23_4kHz    0x00
    #define TMC7300_PWM_FREQ_35_1kHz    0x01
    #define TMC7300_PWM_FREQ_46_9kHz    0x02
    #define TMC7300_PWM_FREQ_58_5kHz    0x03
#define TMC7300_FREEWHEEL_MASK     0x00300000
#define TMC7300_FREEWHEEL_SHIFT    20
    #define TMC7300_STOP_NORMAL         0x00
    #define TMC7300_STOP_FREEWHEEL      0x01
    #define TMC7300_STOP_BRAKE_LS       0x02
    #define TMC7300_STOP_BRAKE_HS       0x03

//Register DRV_STATUS
#define TMC7300_OTPW_MASK          0x00000001
#define TMC7300_OTPW_SHIFT         0
#define TMC7300_OT_MASK            0x00000002
#define TMC7300_OT_SHIFT           1
#define TMC7300_S2GA_MASK          0x00000004
#define TMC7300_S2GA_SHIFT         2
#define TMC7300_S2GB_MASK          0x00000008
#define TMC7300_S2GB_SHIFT         3
#define TMC7300_S2VSA_MASK         0x00000010
#define TMC7300_S2VSA_SHIFT        4
#define TMC7300_S2VSB_MASK         0x00000020
#define TMC7300_S2VSB_SHIFT        5
#define TMC7300_OLA_MASK           0x00000040
#define TMC7300_OLA_SHIFT          6
#define TMC7300_OLB_MASK           0x00000080
#define TMC7300_OLB_SHIFT          7
#define TMC7300_T120_MASK          0x00000100
#define TMC7300_T120_SHIFT         8
#define TMC7300_T150_MASK          0x00000200
#define TMC7300_T150_SHIFT         9

#endif