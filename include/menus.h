/**
 * @file menus.h
 * @author Jim Herd
 * @brief 
 */

//==============================================================================
//  Structure of the menu table
//==============================================================================

#define     MAX_NOS_MODES      16

struct menu {
    uint8_t         nos_modes;
    const char      *mode_strings[MAX_NOS_MODES];
    error_codes_te  (* const mode_function[MAX_NOS_MODES])(uint32_t);
};

extern struct menu test_mode_menu;