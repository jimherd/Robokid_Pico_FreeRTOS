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

#define     MAX_NOS_MODES      16

struct menu {
    bool            primary_menu_mode;
    uint8_t         nos_modes;
    const char      *mode_strings[MAX_NOS_MODES];
    error_codes_te  (* const mode_function[MAX_NOS_MODES])(uint8_t, uint32_t);
    uint32_t        mode_parameter[MAX_NOS_MODES];
};

extern struct menu primary_menu;
extern struct menu gamepad_mode_menu;
extern struct menu bump_mode_menu;
extern struct menu test_mode_menu;
extern struct menu test_mode_2_menu;

error_codes_te  run_menu(struct menu *menu_pt);

#endif