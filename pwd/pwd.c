/*
 * luxOS - a unix-like operating system
 * Omar Elghoul, 2024
 * 
 * echo: implementation of the pwd command
 */

#include <stdio.h>
#include <limits.h>
#include <unistd.h>

int main(void) {
    char path[PATH_MAX];
    printf("%s\n", getwd(path));
    return 0;
}
