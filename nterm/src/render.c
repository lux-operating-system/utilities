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

/* ntermRedrawLine(): redraws one horizontal line
 * params: l - line to redraw
 * returns: nothing
 */

void ntermRedrawLine(int l) {
    uint32_t *back = ((uint32_t *)((uintptr_t)terminal.buffer + (l * 16 * terminal.pitch)));
    uint32_t *front = ((uint32_t *)((uintptr_t)terminal.frame + (l * 16 * terminal.pitch)));
    memcpy(front, back, terminal.lineSize);
}

/* ntermDrawCursor(): draws the cursor on the frame buffer
 * params: none
 * returns: nothing
 */

void ntermDrawCursor() {
    if(!terminal.cursor) return;

    // get pixel offset
    int x = terminal.x * 8;
    int y = terminal.y * 16;
    uint32_t *fb = (uint32_t *)((uintptr_t)terminal.frame + (y * terminal.pitch) + (x * 4));

    // I cursor
    for(int i = 0; i < 16; i++) {
        *fb = terminal.fg;
        fb += terminal.width;
    }
}

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
        uint32_t *secondLine = (uint32_t *)((uintptr_t)terminal.buffer + terminal.lineSize);
        size_t size = (terminal.hchar - 1) * terminal.lineSize;
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

        memcpy(terminal.frame, terminal.buffer, terminal.totalSize);

        if(terminal.cursor) ntermDrawCursor();
        return 1;
    }

    return 0;
}

/* ntermEraseCursor(): erases the cursor from the frame buffer
 * params: none
 * returns: nothing
 */

void ntermEraseCursor() {
    ntermRedrawLine(terminal.y);
}

/* ntermPutc(): draws a character on the frame buffer
 * params: c - character to draw
 * returns: nothing
 */

void ntermPutc(char c) {
    // check for special characters
    if(c == '\e') {
        terminal.escaping = 1;
        memset(terminal.escape, 0, 256);
        terminal.escape[0] = '\e';
        terminal.escapeLen = 1;
        
        return;
    } else if(c == '\n') {             // new line
        ntermEraseCursor();

        terminal.x = 0;
        terminal.y++;
        if(ntermCheckBoundaries()) return;

        ntermDrawCursor();
        return;
    } else if(c == '\r') {      // carriage return
        ntermEraseCursor();
        terminal.x = 0;
        ntermDrawCursor();
        return;
    } else if(c == '\b') {      // backspace
        if(!terminal.x) return;

        terminal.x--;
        ntermPutc(' ');
        terminal.x--;
        ntermEraseCursor();
        ntermDrawCursor();

        return;
    }

    if(terminal.escaping) {
        terminal.escape[terminal.escapeLen] = c;
        terminal.escapeLen++;

        if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
            terminal.escaping = 0;
            parseEscape();
        }

        return;
    }

    if(c < FONT_MIN_GLYPH || c > FONT_MAX_GLYPH)
        return ntermPutc('?');

    // erase the old cursor
    //ntermEraseCursor();

    // get pixel offset
    int x = terminal.x * 8;
    int y = terminal.y * 16;
    uint32_t *back = (uint32_t *)((uintptr_t)terminal.buffer + (y * terminal.pitch) + (x * 4));
    uint32_t *front = (uint32_t *)((uintptr_t)terminal.frame + (y * terminal.pitch) + (x * 4));

    // font data
    const uint8_t *data = &font[(c - FONT_MIN_GLYPH)*16];

    for(int i = 0; i < 16; i++) {
        uint8_t b = data[i];

        for(int j = 0; j < 8; j++) {
            if(b & 0x80) back[j] = terminal.fg;
            else back[j] = terminal.bg;

            front[j] = back[j];
            b <<= 1;
        }

        back = (uint32_t *)((uintptr_t)back + terminal.pitch);
        front = (uint32_t *)((uintptr_t)front + terminal.pitch);
    }

    //ntermRedrawLine(terminal.y);

    // and advance the cursor
    terminal.x++;
    if(ntermCheckBoundaries()) return;

    ntermDrawCursor();
}

/* ntermPutcn(): renders a string of n characters on the screen
 * params: s - pointer to string
 * params: n - number of characters to print
 * returns: nothing
 */

void ntermPutcn(const char *s, size_t n) {
    if(!n) return;

    for(size_t i = 0; i < n; i++)
        ntermPutc(s[i]);
}