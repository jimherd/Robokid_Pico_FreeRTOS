/**
 * @file run_test_modes.h
 * @author Jim Herd
 * @brief 
 */

#ifndef __RUN_TEST_MODES_H__
#define __RUN_TEST_MODES_H__

error_codes_te run_test_modes(uint8_t mode_index, uint32_t parameter);
error_codes_te run_test_0(uint8_t mode_index, uint32_t parameter);
error_codes_te run_test_1(uint8_t mode_index, uint32_t parameter);
error_codes_te run_test_2(uint8_t mode_index, uint32_t parameter);
error_codes_te run_test_3(uint8_t mode_index, uint32_t parameter);
error_codes_te run_test_2_menu(uint8_t mode_index, uint32_t parameter);

#endif  /* __RUN_TEST_MODES_H__  */