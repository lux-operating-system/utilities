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
#include <stdio.h>

void parseCSI() {
    char *seq = (char *) terminal.escape;
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
        } else if(seq[2] == '2' || seq[2] == '3') {
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
    
    case 'H':       // move cursor
        seq += 2;
        int x = 0, y = 0;
        ntermEraseCursor();

        token = strtok((char *) seq, ";");
        if(!token) {
            x = 0;
            if(seq[0] >= '0' && seq[0] <= '9') y = atoi(seq) - 1;
            else y = 0;
        } else {
            y = atoi(token) - 1;
            token = strtok(NULL, ";");
            if(!token || !strlen(token)) y = 0;
            else x = atoi(token) - 1;
        }

        if(y >= terminal.hchar-1) y = terminal.hchar-1;
        else if(y < 0) y = 0;

        if(x >= terminal.wchar-1) x = terminal.wchar-1;
        else if(x < 0) x = 0;

        terminal.x = x;
        terminal.y = y;
        ntermDrawCursor();
        break;
    
    case 'A':       // cursor up
        int up;
        if(seq[2] >= '0' && seq[2] <= '9') up = atoi(&seq[2]);
        else up = 1;

        ntermEraseCursor();
        terminal.y -= up;
        if(terminal.y < 0) terminal.y = 0;
        ntermDrawCursor();
        break;
    
    case 'B':       // cursor down
        int down;
        if(seq[2] >= '0' && seq[2] <= '9') down = atoi(&seq[2]);
        else down = 1;

        ntermEraseCursor();
        terminal.y += down;
        if(terminal.y >= terminal.hchar) terminal.y = terminal.hchar-1;
        ntermDrawCursor();
        break;
    
    case 'C':       // cursor forward
        int right;
        if(seq[2] >= '0' && seq[2] <= '9') right = atoi(&seq[2]);
        else right = 1;

        ntermEraseCursor();
        terminal.x += right;
        if(terminal.x >= terminal.wchar) terminal.x = terminal.wchar-1;
        ntermDrawCursor();
        break;
    
    case 'D':       // cursor backward
        int left;
        if(seq[2] >= '0' && seq[2] <= '9') left = atoi(&seq[2]);
        else left = 1;

        ntermEraseCursor();
        terminal.x -= left;
        if(terminal.x < 0) terminal.x = 0;
        ntermDrawCursor();
        break;
    
    case 'E':       // cursor next line
        int next;
        if(seq[2] >= '0' && seq[2] <= '9') next = atoi(&seq[2]);
        else next = 1;

        ntermEraseCursor();
        terminal.x = 0;
        terminal.y += next;
        if(terminal.y >= terminal.hchar) terminal.y = terminal.hchar-1;
        ntermDrawCursor();
        break;
    
    case 'F':       // cursor previous line
        int prev;
        if(seq[2] >= '0' && seq[2] <= '9') prev = atoi(&seq[2]);
        else prev = 1;

        ntermEraseCursor();
        terminal.x = 0;
        terminal.y -= prev;
        if(terminal.y < 0) terminal.y = 0;
        ntermDrawCursor();
        break;
    
    case 'n':       // report cursor position
        if(seq[2] != '6') break;

        char buffer[64];
        sprintf(buffer, "\e[%d;%dR", terminal.y+1, terminal.x+1);
        write(terminal.master, buffer, strlen(buffer));
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