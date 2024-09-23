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

int child(char *slavepty) {
    // open the slave pty for stdin, out, err
    for(int i = 0; i < 3; i++) {
        if(open(slavepty, O_RDWR) < 0) exit(-1);
    }

    while(1);
}

int main(int argc, char **argv) {
    // this is the first non-server process to run on lux
    // we need to set up stdio for all children processes
    // start by creating a terminal
    int master = posix_openpt(O_RDWR | O_CLOEXEC);
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
    terminal.kbd = open("/dev/kbd", O_RDONLY | O_CLOEXEC);
    if(terminal.lfb < 0 || terminal.kbd < 0) return -1;

    // get the screen resolution
    unsigned long temp;
    if(ioctl(terminal.lfb, LFB_GET_WIDTH, &temp)) return -1;
    terminal.width = temp;
    if(ioctl(terminal.lfb, LFB_GET_HEIGHT, &temp)) return -1;
    terminal.height = temp;
    terminal.pixelCount = terminal.width * terminal.height;

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
    while(1);

    // fork and spawn a test process
    pid_t pid = fork();
    if(pid < 0) return -1;
    if(!pid) return child(slaveName);

    while(1);
}