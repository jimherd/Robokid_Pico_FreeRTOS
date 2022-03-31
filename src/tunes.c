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

struct note_data_s test_notes[3] = {
        {NOTE_C, 10},
        {SILENT_NOTE, 5},
        {NOTE_B, 10}
};