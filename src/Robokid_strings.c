
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
    "No mode       "
    "Mode Jstick   ",
    "Mode Activity ",
    "Mode Bump     ",
    "Mode Follow   ",
    "Mode Program  ",
    "Mode Sketch   ",
    "Mode Lab      ",
    "Mode Distance ",
    "Mode Exp      "
};

const char mode_J[16] = {
    "Mode - J   ",
};

const char mode_tank[16] = {
    "Jmode: Tank   ",
};

const char* top_level_button_data[4] = {
    "A: Accept     ",
    "B: Next mode  ",
    "C: -----      ",
    "D: -----      "
};

const char* mode_J_button_data[4] = {
    "A: go/stop    ",
    "B: Next mode  ",
    "C: -----      ",
    "D: Exit mode  "  
};