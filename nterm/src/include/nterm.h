/*
 * luxOS - a unix-like operating system
 * Omar Elghoul, 2024
 * 
 * nterm: Minimalist (nano) terminal emulator that uses /dev/lfbX for output
 * and /dev/kbd as input
 */

#pragma once

#include <stdint.h>

#define FONT_MIN_GLYPH          32      // ' '
#define FONT_MAX_GLYPH          126     // '~'

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
} TerminalStatus;

extern TerminalStatus terminal;
extern const uint8_t font[];

void ntermPutc(char);
