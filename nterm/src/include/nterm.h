/*
 * luxOS - a unix-like operating system
 * Omar Elghoul, 2024
 * 
 * nterm: Minimalist (nano) terminal emulator that uses /dev/lfbX for output
 * and /dev/kbd as input
 */

#pragma once

#include <stdint.h>
#include <stddef.h>

#define DEFAULT_SCANCODES       84      // count

#define FONT_MIN_GLYPH          32      // ' '
#define FONT_MAX_GLYPH          126     // '~'

#define BUFFER_SIZE             1024

typedef struct {
    int primary;            // primary terminal descriptor
    int width, height;      // pixels
    int wchar, hchar;       // characters
    int x, y;               // cursor position
    int pitch;
    uint32_t *buffer;       // back buffer
    uint32_t *frame;        // front frame buffer
    int lfb, kbd;           // file descriptors
    int pixelCount;
    int lineSize;           // bytes
    int totalSize;          // bytes
    int cursor;             // cursor visibility
    uint32_t bg, fg;

    int escaping;           // 1 if parsing escape sequence
    uint8_t escape[256];    // escape sequence buffer
    int escapeLen;          // length of escape sequence

    int keyCount;
    uint16_t scancodes[BUFFER_SIZE];    // keyboard scan codes
    uint8_t printableKeys[BUFFER_SIZE]; // readable key presses

    int outputCount;
    uint8_t output[BUFFER_SIZE];
} TerminalStatus;

extern TerminalStatus terminal;
extern const uint8_t font[];
extern const char scancodesDefault[];
extern const char scancodesDefaultShift[];
extern const uint32_t ttyColors[];

void ntermPutc(char);
void ntermPutcn(const char *, size_t);
void ntermPuts(const char *);
void parseEscape();
void ntermRedrawLine(int);
void ntermDrawCursor();
void ntermEraseCursor();
