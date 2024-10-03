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
#include <stdlib.h>

void parseCSI() {
    // check what command this is
    uint8_t *seq = (uint8_t *) terminal.escape;
    char *token;

    switch(seq[terminal.escapeLen-1]) {
    case 'm':
        // set colors/font attributes
        seq += 2;   // skip to first attribute
        token = strtok((char *) seq, ";");
        while(token) {
            int n = atoi(token);
            if(!n) {
                terminal.bg = ttyColors[0];
                terminal.fg = ttyColors[7];
            } else if(n >= 30 && n <= 37) {
                terminal.fg = ttyColors[n - 30];
            } else if(n >= 40 && n <= 47) {
                terminal.bg = ttyColors[n - 40];
            } else if(n >= 90 && n <= 97) {
                terminal.fg = ttyColors[n - 83];    // bright
            } else if(n >= 100 && n <= 107) {
                terminal.bg = ttyColors[n - 93];
            }

            token = strtok(NULL, ";");
        }

        break;
    }
}

void parseEscape() {
    switch(terminal.escape[1]) {
    case '[':   // CSI
        return parseCSI();
    }
}