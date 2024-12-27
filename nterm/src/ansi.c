/*
 * luxOS - a unix-like operating system
 * Omar Elghoul, 2024
 * 
 * nterm: Minimalist (nano) terminal emulator that uses /dev/lfbX for output
 * and /dev/kbd as input
 */

/* Implementation of ANSI Control Sequences */

#include <nterm.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void parseCSI() {
    uint8_t *seq = (uint8_t *) terminal.escape;
    char *token;

    int n;

    switch(seq[terminal.escapeLen-1]) {
    case 'm':       // set rendering attributes
        seq += 2;   // skip to first attribute
        token = strtok((char *) seq, ";");
        while(token) {
            n = atoi(token);
            if(!n) {
                terminal.bg = ttyColors[0];
                terminal.fg = ttyColors[7];
            } else if(n >= 30 && n <= 37) {
                terminal.fg = ttyColors[n - 30];
            } else if(n >= 40 && n <= 47) {
                terminal.bg = ttyColors[n - 40];
            } else if(n >= 90 && n <= 97) {
                terminal.fg = ttyColors[n - 82];    // bright
            } else if(n >= 100 && n <= 107) {
                terminal.bg = ttyColors[n - 92];
            } else if(n == 39) {    // default foreground
                terminal.fg = ttyColors[7];
            } else if(n == 49) {    // default background
                terminal.bg = ttyColors[0];
            }

            token = strtok(NULL, ";");
        }

        break;
    
    case 'J':       // erase
        if(seq[2] == '1') {
            /* clear from beginning of screen to cursor position */
            if(terminal.y) {
                for(int i = 0; i < (terminal.lineSize / 4) * terminal.y; i++) {
                    terminal.buffer[i] = terminal.bg;
                }
            }

            if(terminal.x) {
                int oldx = terminal.x;
                int oldy = terminal.y;

                terminal.x = 0;

                for(int i = 0; i < terminal.x; i++)
                    ntermPutc(' ');
                
                terminal.x = oldx;
                terminal.y = oldy;
            }

            for(int i = 0; i < terminal.y; i++) {
                ntermRedrawLine(i);
            }

            ntermDrawCursor();
        } else if(seq[2] == '2') {
            /* clear entire screen */
            for(int i = 0; i < terminal.totalSize / 4; i++) {
                terminal.buffer[i] = terminal.bg;
            }

            for(int i = 0; i < terminal.y; i++) {
                ntermRedrawLine(i);
            }

            ntermDrawCursor();
        }

        break;
    }
}

void parseVT220() {
    uint8_t *seq = (uint8_t *) terminal.escape;
    int n;

    switch(seq[terminal.escapeLen-1]) {
    case 'h':       // enable cursor/focus
        n = atoi((const char *) seq+2);
        if(n == 25) terminal.cursor = 1;
        break;

    case 'l':       // disable cursor/focus
        n = atoi((const char *) seq+2);
        if(n == 25) terminal.cursor = 0;
        break;
    }
}

void parseEscape() {
    switch(terminal.escape[1]) {
    case '[': return parseCSI();
    case '?': return parseVT220();
    }
}