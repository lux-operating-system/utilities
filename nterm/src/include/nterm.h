/*
 * luxOS - a unix-like operating system
 * Omar Elghoul, 2024
 * 
 * nterm: Minimalist (nano) terminal emulator that uses /dev/lfbX for output
 * and /dev/kbd as input
 */

#pragma once

#include <stdint.h>

#define DEFAULT_SCANCODES       84      // count

#define FONT_MIN_GLYPH          32      // ' '
#define FONT_MAX_GLYPH          126     // '~'

#define BUFFER_SIZE             64

typedef struct {
    int width, height;      // pixels
    int wchar, hchar;       // characters
    int x, y;               // cursor position
    int pitch;
    uint32_t *buffer;       // frame buffer
    int lfb, kbd;           // file descriptors
    int pixelCount;
    int lineSize;           // bytes
    int totalSize;          // bytes
    int cursor;             // cursor visibility
    int echo;               // keypress echo
    uint32_t bg, fg;

    int keyCount;
    uint16_t scancodes[BUFFER_SIZE];    // keyboard scan codes
    uint8_t printableKeys[BUFFER_SIZE]; // readable key presses

    int slaveCount;
    uint8_t slaveOutput[BUFFER_SIZE];   // output of the slave
} TerminalStatus;

extern TerminalStatus terminal;
extern const uint8_t font[];
extern const char scancodesDefault[];
extern const char scancodesDefualtShift[];

void ntermPutc(char);
