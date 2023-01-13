/**
 * @file run_bump_modes.h
 * @author Jim Herd (you@domain.com)
 * @brief 
 */

#ifndef __RUN_BUMP_MODES_H__
#define __RUN_BUMP_MODES_H__

error_codes_te run_bump_modes(uint8_t mode_index, uint32_t parameter);
error_codes_te run_bump_mode_0(uint8_t mode_index, uint32_t  parameter);
error_codes_te run_bump_mode_1(uint8_t mode_index, uint32_t  parameter);
error_codes_te run_bump_mode_2(uint8_t mode_index, uint32_t  parameter);

#endif  /* __RUN_BUMP_MODES_H__  */