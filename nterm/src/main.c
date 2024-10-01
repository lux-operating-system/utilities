/*
 * luxOS - a unix-like operating system
 * Omar Elghoul, 2024
 * 
 * nterm: Minimalist (nano) terminal emulator that uses /dev/lfbX for output
 * and /dev/kbd as input
 */

#include <fnctl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/lux/lux.h>
#include <liblux/lfb.h>
#include <nterm.h>
#include <stdio.h>

TerminalStatus terminal;

const uint32_t ttyColors[] = {
    0x1F1F1F,       // black
    0x990000,       // red
    0x00A600,       // green
    0x999900,       // yellow
    0x0000B2,       // blue
    0xB200B2,       // magenta
    0x00A6B2,       // cyan
    0xBFBFBF,       // white
    0x666666,       // gray
    0xE60000,       // bright red
    0x00D900,       // bright green
    0xE6E600,       // bright yellow
    0x0000FF,       // bright blue
    0xE600E6,       // bright magenta
    0x00E6E6,       // bright cyan
    0xE6E6E6        // bright white
};

int child(char *slavepty, int maxfd, int argc, char **argv) {
    if(maxfd) {
        for(int i = 0; i < maxfd; i++) close(i);
    }

    // open the slave pty for stdin, out, err
    for(int i = 0; i < 3; i++) {
        if(open(slavepty, O_RDWR) < 0) return -1;
    }

    if(argc >= 2)
        execvp(argv[1], &argv[1]);

    while(1);
}

int main(int argc, char **argv) {
    // this is the first non-server process to run on lux
    // we need to set up stdio for all children processes
    // start by creating a terminal
    int master = posix_openpt(O_RDWR | O_NONBLOCK | O_CLOEXEC);
    if(master < 0) return -1;

    grantpt(master);
    unlockpt(master);

    char *tempSlave = ptsname(master);
    if(!tempSlave) return -1;
    char *slaveName = malloc(strlen(tempSlave) + 1);
    if(!slaveName) return -1;
    strcpy(slaveName, tempSlave);

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
    terminal.pitch = terminal.width * 4;
    terminal.lineSize = terminal.pitch * 16;
    terminal.totalSize = terminal.lineSize * terminal.height;
    terminal.cursor = 1;
    terminal.echo = 1;
    terminal.bg = ttyColors[0];
    terminal.fg = ttyColors[7];
    terminal.keyCount = 0;
    terminal.slaveCount = 0;
    terminal.redraw = 1;

    terminal.wchar = terminal.width / 8;
    terminal.hchar = terminal.height / 8;
    terminal.x = 0;
    terminal.y = 0;
    terminal.buffer = malloc(terminal.pixelCount * 4);
    if(!terminal.buffer) return -1;

    // clear the screen
    for(int i = 0; i < terminal.width*terminal.height; i++)
        terminal.buffer[i] = ttyColors[0];

    write(terminal.lfb, terminal.buffer, terminal.pixelCount * 4);

    // fork and spawn a test process
    pid_t pid = fork();
    if(pid < 0) return -1;
    if(!pid) return child(slaveName, terminal.kbd, argc, argv);

    // idle loop where we read from the keyboard and write to the master pty,
    // and then read from the master pty and draw to the screen
    for(;;) {
        // read from the keyboard
        ssize_t s = read(terminal.kbd, terminal.scancodes, BUFFER_SIZE*2);
        if(s >= 2 && s <= BUFFER_SIZE) {
            size_t events = s/2;

            // only save the key presses, not the key ups
            terminal.keyCount = 0;
            for(int i = 0; i < events; i++) {
                if(!(terminal.scancodes[i] & 0x8000) && (terminal.scancodes[i] < DEFAULT_SCANCODES)) {
                    terminal.printableKeys[terminal.keyCount] = scancodesDefault[terminal.scancodes[i]];
                    if(terminal.echo && terminal.printableKeys[terminal.keyCount])
                        ntermPutc(terminal.printableKeys[terminal.keyCount]);

                    terminal.keyCount++;
                }
            }

            // and send the key presses to the terminal
            write(master, terminal.printableKeys, terminal.keyCount);
        }

        // read from the terminal to draw on the screen
        s = read(master, terminal.slaveOutput, BUFFER_SIZE);
        if(s > 0 && s <= BUFFER_SIZE) {
            terminal.slaveCount = s;
            ntermPutcn((const char *)terminal.slaveOutput, terminal.slaveCount);
        }
    }
}