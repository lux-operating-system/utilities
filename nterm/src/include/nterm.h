/*
 * luxOS - a unix-like operating system
 * Omar Elghoul, 2024
 * 
 * nterm: Minimalist (nano) terminal emulator that uses /dev/lfbX for output
 * and /dev/kbd as input
 */

#pragma once

typedef struct {
    int width, height;      // pixels
    int wchar, hchar;       // characters
    int x, y;               // cursor position
    uint32_t *buffer;       // frame buffer
    int lfb, kbd;           // file descriptors
    int pixelCount;
} TerminalStatus;

extern TerminalStatus terminal;
