
#ifndef __SOUND_H__
#define __SOUND_H__

#include <stdio.h>

//
// Frequencies of octave 4
//
#define     FREQ_C      262
#define     FREQ_CS     277
#define     FREQ_D      294
#define     FREQ_DS     311
#define     FREQ_E      330
#define     FREQ_F      349
#define     FREQ_FS     370
#define     FREQ_G      382
#define     FREQ_GS     415
#define     FREQ_A      440
#define     FREQ_AS     455
#define     FREQ_B      484


#define     NOTE_C      0
#define     NOTE_CS     1
#define     NOTE_D      2
#define     NOTE_DS     3
#define     NOTE_E      4
#define     NOTE_F      5
#define     NOTE_FS     6
#define     NOTE_G      7
#define     NOTE_GS     8
#define     NOTE_A      9
#define     NOTE_AS     10
#define     NOTE_B      11
#define     SILENT_NOTE 12

uint8_t     notes[13] = {
    FREQ_C, FREQ_CS, FREQ_D, FREQ_DS, FREQ_E, FREQ_F,
    FREQ_FS, FREQ_G, FREQ_GS, FREQ_A, FREQ_AS, FREQ_B, SILENT_NOTE
};

typedef struct  {
    uint8_t   mode, note_count;
    uint8_t   note[32][2];
} sound_file_t;

enum {SOUND_DISABLE, SOUND_ENABLE};

void sound_init(void);
void play_tune(const sound_file_t  *sound_file_pt);
void stop_tune(void);
void set_tone(uint8_t note, uint16_t duration);
void tone_on(uint8_t note);
void tone_off(void);
void beep(uint8_t note, uint8_t count);

#endif