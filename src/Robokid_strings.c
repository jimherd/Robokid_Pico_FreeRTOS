
#include    "system.h"
#include    "Robokid_strings.h"


const char    blank_row[16] = {"              "};

const char* system_busy[4] = {
    "Robokid |     ",
    "Robokid /     ",
    "Robokid -     ",
    "Robokid \\     ",
};

const char* system_ready[1] = {
    " System ready ", 
};

const char* test_scroll_string_data[4] = {
    "sw.A: Exit    ",
    "sw.B: Start   ",
    "sw.C: Reset   ",
    "sw.D: Stop    "
};

const char* wait_start[1] = {
    "A: Start      ",
};

const char* main_modes[NOS_PRIMARY_MODES+1] = {
    "No mode       ",
    "Mode Gamepad  ",
    "Mode Activity ",
    "Mode Bump     ",
    "Mode Follow   ",
    "Mode Program  ",
    "Mode Sketch   ",
    "Mode Lab      ",
    "Mode Distance ",
    "Mode Exp      "
};

const char* gamepad_connect[3] = {
    "Please connect",
    "GAMPAD to USB ",  
    "A: When done  ",
};

const char* mode_G[2] = {
    "   G mode 1   ",
    "   G mode 2   ",
};

const char* top_level_button_data[2] = {
    "A: Accept     ",
    "B: Next mode  ",
};

const char* mode_G_button_data[4] = {
    "A: go/stop    ",
    "B: Next mode  ",
    "C: -----      ",
    "D: Exit mode  "  
};

const char* blank_scroll_area[2] = {
    "              ",
    "              ",  
};