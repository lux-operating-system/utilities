/*
 * luxOS - a unix-like operating system
 * Omar Elghoul, 2024
 * 
 * nterm: Minimalist (nano) terminal emulator that uses /dev/lfbX for output
 * and /dev/kbd as input
 */

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/lux/lux.h>
#include <liblux/lfb.h>
#include <liblux/kbd.h>
#include <nterm.h>
#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <termios.h>
#include <errno.h>

TerminalStatus terminal;

int child(char *secondarypty, int maxfd, int argc, char **argv) {
    if(maxfd) {
        for(int i = 0; i <= maxfd; i++) close(i);
    }

    // open the secondary pty for stdin, out, err
    for(int i = 0; i < 3; i++) {
        if(open(secondarypty, O_RDWR) < 0) return -1;
    }

    if(argc >= 2) {
        execvp(argv[1], &argv[1]);  // execute the second argument by default
        fprintf(stderr, "nterm: could not execute %s, trying lush...\n", argv[1]);
    }
    
    // if that doesn't work, execute the default shell
    char *args[] = { "lush", NULL };
    execvp("lush", args);
    fprintf(stderr, "nterm: could not execute lush, aborting...\n");

    exit(-1);
}

int main(int argc, char **argv) {
    // this is the first non-server process to run on lux
    // we need to set up stdio for all children processes
    // start by creating a terminal
    memset(&terminal, 0, sizeof(TerminalStatus));

    terminal.primary = posix_openpt(O_RDWR | O_NONBLOCK | O_CLOEXEC);
    if(terminal.primary < 0) return -1;

    grantpt(terminal.primary);
    unlockpt(terminal.primary);

    char *tempSecondary = ptsname(terminal.primary);
    if(!tempSecondary) return -1;
    char *secondaryName = malloc(strlen(tempSecondary) + 1);
    if(!secondaryName) return -1;
    strcpy(secondaryName, tempSecondary);

    // open the frame buffer and keyboard
    terminal.lfb = open("/dev/lfb0", O_RDWR | O_CLOEXEC);
    terminal.kbd = open("/dev/kbd", O_RDONLY | O_NONBLOCK | O_CLOEXEC);
    if(terminal.lfb < 0 || terminal.kbd < 0) return -1;

    // get the screen resolution
    unsigned long temp;
    if(ioctl(terminal.lfb, LFB_GET_WIDTH, &temp)) return -1;
    terminal.width = temp;
    if(ioctl(terminal.lfb, LFB_GET_HEIGHT, &temp)) return -1;
    terminal.height = temp;
    terminal.pixelCount = terminal.width * terminal.height;
    if(ioctl(terminal.lfb, LFB_GET_PITCH, &temp)) return -1;
    terminal.pitch = temp;
    terminal.lineSize = terminal.pitch * 16;
    terminal.totalSize = terminal.pitch * terminal.height;
    terminal.cursor = 1;
    terminal.bg = ttyColors[0];
    terminal.fg = ttyColors[7];

    terminal.wchar = terminal.width / 8;
    terminal.hchar = terminal.height / 16;
    terminal.x = 0;
    terminal.y = 0;
    terminal.buffer = malloc(terminal.totalSize);
    if(!terminal.buffer) return -1;
    terminal.frame = mmap(NULL, terminal.totalSize, PROT_READ | PROT_WRITE, MAP_SHARED, terminal.lfb, 0);
    if(terminal.frame == MAP_FAILED) return -1;

    // clear the screen
    for(int i = 0; i < terminal.totalSize/4; i++)
        terminal.buffer[i] = ttyColors[0];

    for(int i = 0; i < terminal.hchar; i++)
        ntermRedrawLine(i);
    
    // let the pty driver know the terminal's size
    struct winsize ws;
    if(tcgetwinsize(terminal.primary, &ws)) {
        ntermPuts("failed to request terminal window size: ");
        ntermPuts(strerror(errno));
        for(;;);
    }

    ws.ws_col = terminal.wchar;
    ws.ws_row = terminal.hchar;
    if(tcsetwinsize(terminal.primary, &ws)) {
        ntermPuts("failed to set terminal window size: ");
        ntermPuts(strerror(errno));
        for(;;);
    }

    // set controlling terminal
    tcsetpgrp(terminal.primary, getpid());
    setpgrp();

    setenv("TERM", "xterm-16color", 1);

    // fork and spawn a test process
    pid_t pid = fork();
    if(pid < 0) return -1;
    if(!pid) return child(secondaryName, terminal.kbd, argc, argv);

    int shift = 0, control = 0;
    int busy;

    // idle loop where we read from the keyboard and write to the primary pty,
    // and then read from the primary pty and draw to the screen
    for(;;) {
        busy = 0;

        // read from the keyboard
        ssize_t s = read(terminal.kbd, terminal.scancodes, BUFFER_SIZE*2);
        if(s >= 2 && s <= BUFFER_SIZE) {
            busy = 1;
            size_t events = s/2;

            // only save the key presses, not the key ups
            char *scancodeLookup;

            for(int i = 0; i < events; i++) {
                if((terminal.scancodes[i] == KBD_KEY_LEFT_SHIFT) || (terminal.scancodes[i] == KBD_KEY_RIGHT_SHIFT)) {
                    shift = 1;
                    continue;
                } else if((terminal.scancodes[i] == (KBD_KEY_LEFT_SHIFT | KBD_KEY_RELEASE)) ||
                    (terminal.scancodes[i] == (KBD_KEY_RIGHT_SHIFT | KBD_KEY_RELEASE))) {
                    shift = 0;
                    continue;
                } else if((terminal.scancodes[i] == KBD_KEY_LEFT_CTRL || (terminal.scancodes[i] == KBD_RIGHT_CTRL))) {
                    control = 1;
                    continue;
                } else if((terminal.scancodes[i] == (KBD_KEY_LEFT_CTRL | KBD_KEY_RELEASE)) ||
                    (terminal.scancodes[i] == (KBD_RIGHT_CTRL | KBD_KEY_RELEASE))) {
                    control = 0;
                    continue;
                }

                if(shift) scancodeLookup = (char *) scancodesDefaultShift;
                else scancodeLookup = (char *) scancodesDefault;

                if(!(terminal.scancodes[i] & 0x8000) && (terminal.scancodes[i] < DEFAULT_SCANCODES)) {
                    terminal.printableKeys[terminal.keyCount] = scancodeLookup[terminal.scancodes[i]];
                    if(terminal.printableKeys[terminal.keyCount]) {
                        if(control) {
                            /* control characters */
                            char c = tolower(terminal.printableKeys[terminal.keyCount]);
                            switch(c) {
                            case 'c':   // Ctrl+C
                                c = 0x03;   // end of text
                                write(terminal.primary, &c, 1);
                                break;
                            
                            case 'd':   // Ctrl+D
                                c = 0x04;   // end of transmission
                                write(terminal.primary, &c, 1);
                                break;
                            }
                        } else {
                            terminal.keyCount++;
                        }
                    }
                }
            }

            // send the key presses to the children processes on this terminal
            if(terminal.keyCount) {
                write(terminal.primary, terminal.printableKeys, terminal.keyCount);
            }
            memset(terminal.printableKeys, 0, sizeof(terminal.printableKeys));
            terminal.keyCount = 0;
        }

        // read from the terminal to draw on the screen
        s = read(terminal.primary, terminal.output, BUFFER_SIZE);
        if(s > 0 && s <= BUFFER_SIZE) {
            busy = 1;
            terminal.outputCount = s;
            ntermPutcn((const char *)terminal.output, terminal.outputCount);
        }

        if(!busy) sched_yield();
    }
}