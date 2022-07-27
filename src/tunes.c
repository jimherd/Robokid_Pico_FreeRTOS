/**
 * @brief Declared set of tunes
 * 
 */

#include    "system.h"
#include    "tunes.h"

struct note_data_s beep[2] = {
        {NOTE_C, 5},
        {SILENT_NOTE, 5},
};

struct  note_data_s next_mode[4] = {
        {NOTE_F, 2},
        {SILENT_NOTE, 1},
        {NOTE_G, 2},
        {SILENT_NOTE, 2},
};

struct note_data_s last_mode[4] = {
        {NOTE_F, 2},
        {SILENT_NOTE, 1},
        {NOTE_E, 2},
        {SILENT_NOTE, 2},
};

struct note_data_s test_notes[3] = {
        {NOTE_C, 10},
        {SILENT_NOTE, 5},
        {NOTE_B, 10}
};