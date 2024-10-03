/*
 * luxOS - a unix-like operating system
 * Omar Elghoul, 2024
 * 
 * echo: implementation of the echo command
 */

#include <stdio.h>

int main(int argc, char **argv) {
    for(int i = 1; (argc >= 2) && (i < argc); i++)
        printf("%s ", argv[i]);
    
    printf("\n");
    return 0;
}
