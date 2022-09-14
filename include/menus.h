/**
 * @file menus.h
 * @author Jim Herd
 * @brief 
 */

//==============================================================================
//  Structure of the menu table
//==============================================================================

#ifndef __MENUS_H__
#define __MENUS_H__

#define     NOS_MODE_PARAMETERS      2

typedef error_codes_te (*mode_function)(uint8_t, uint32_t); 

struct menu {
    uint8_t         nos_modes;
    const char**    mode_strings;
    mode_function*  func;
    int32_t         mode_parameter[NOS_MODE_PARAMETERS];
};

extern struct menu menu_1;
extern struct menu menu_1_1;
extern struct menu menu_1_3;
extern struct menu menu_1_9;
extern struct menu menu_1_9_2;

error_codes_te  run_menu(struct menu *menu_pt);

#endif

// struct menu_old {
//     bool            primary_menu_mode;
//     uint8_t         nos_modes;
//     const char      *mode_strings[MAX_NOS_MODES];
//     error_codes_te  (* const mode_function[MAX_NOS_MODES])(uint8_t, uint32_t);
//    uint32_t        mode_parameter[MAX_NOS_MODES];
// };

// extern struct menu primary_menu;
// extern struct menu gamepad_mode_menu;
// extern struct menu bump_mode_menu;
// extern struct menu test_mode_menu;
// extern struct menu test_mode_2_menu;