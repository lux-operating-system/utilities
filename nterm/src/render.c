/*
 * luxOS - a unix-like operating system
 * Omar Elghoul, 2024
 * 
 * nterm: Minimalist (nano) terminal emulator that uses /dev/lfbX for output
 * and /dev/kbd as input
 */

#include <nterm.h>
#include <unistd.h>
#include <string.h>

/* ntermCheckBoundaries(): checks the boundaries and scrolls if necessary
 * params: none
 * returns: 1 if scrolled, zero if not
 */

int ntermCheckBoundaries() {
    if(terminal.x >= terminal.wchar) {
        terminal.x = 0;
        terminal.y++;
    }

    if(terminal.y >= terminal.hchar) {
        // scroll up by one line
        uint32_t *secondLine = (uint32_t *)((uintptr_t)terminal.buffer + (16*terminal.pitch));
        size_t size = (terminal.hchar - 1) * 16 * terminal.pitch;
        memcpy(terminal.buffer, secondLine, size);

        // clear the scrolled line, which is also pointed to by size
        uint32_t *lastLine = (uint32_t *)((uintptr_t)terminal.buffer + size);
        for(int i = 0; i < 16; i++) {
            for(int j = 0; j < terminal.width; j++) {
                lastLine[j] = terminal.bg;
            }

            lastLine = (uint32_t *)((uintptr_t)lastLine + terminal.pitch);
        }

        terminal.x = 0;
        terminal.y = terminal.hchar - 1;

        // and update the entire screen
        lseek(terminal.lfb, 0, SEEK_SET);
        write(terminal.lfb, terminal.buffer, terminal.totalSize);
        return 1;
    }

    return 0;
}

/* ntermPutc(): draws a character on the frame buffer
 * params: c - character to draw
 * returns: nothing
 */

void ntermPutc(char c) {
    // check for special characters
    if(c == '\n') {             // new line
        terminal.x = 0;
        terminal.y++;
        ntermCheckBoundaries();
        return;
    } else if(c == '\r') {      // carriage return
        terminal.x = 0;
        return;
    }

    // get pixel offset
    int x = terminal.x * 8;
    int y = terminal.y * 16;
    uint32_t *fb = (uint32_t *)((uintptr_t)terminal.buffer + (y * terminal.pitch) + (x * 4));

    // font data
    const uint8_t *data = &font[(c - FONT_MIN_GLYPH)*16];

    for(int i = 0; i < 16; i++) {
        uint8_t b = data[i];

        for(int j = 0; j < 8; j++) {
            if(b & 0x80) fb[j] = terminal.fg;
            else fb[j] = terminal.bg;

            b <<= 1;
        }

        fb = (uint32_t *)((uintptr_t)fb + terminal.pitch);
    }

    // and update the frame buffer
    uint32_t *ptr = (uint32_t *)((uintptr_t)terminal.buffer + (y * terminal.pitch));
    off_t offset = y * terminal.pitch;

    // and advance the cursor
    terminal.x++;
    if(ntermCheckBoundaries()) return;

    lseek(terminal.lfb, offset, SEEK_SET);
    write(terminal.lfb, ptr, terminal.lineSize);
}