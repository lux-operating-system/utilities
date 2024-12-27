/*
 * luxOS - a unix-like operating system
 * Omar Elghoul, 2024
 * 
 * reset: implementation of the reset command
 */

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>

#define DEFAULT_IFLAG           (ICRNL | IGNCR | IGNPAR)
#define DEFAULT_OFLAG           (ONLRET)
#define DEFAULT_CFLAG           (CS8 | HUPCL)
#define DEFAULT_LFLAG           (ECHO | ECHOE | ECHOK | ECHONL | ICANON)

int main(void) {
    struct termios termios;
    if(tcgetattr(fileno(stdin), &termios)) return EXIT_FAILURE;

    termios.c_cflag = DEFAULT_CFLAG;
    termios.c_iflag = DEFAULT_IFLAG;
    termios.c_oflag = DEFAULT_OFLAG;
    termios.c_lflag = DEFAULT_LFLAG;
    if(tcsetattr(fileno(stdin), TCSANOW, &termios)) return EXIT_FAILURE;

    printf("\e[0m\e[2J\e[1;1H");
    fflush(stdout);
    return EXIT_SUCCESS;
}
