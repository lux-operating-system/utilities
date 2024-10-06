/*
 * luxOS - a unix-like operating system
 * Omar Elghoul, 2024
 * 
 * luxfetch: neofetch clone for luxOS
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define COLOR       "\e[0;94m"
#define RESET       "\e[0m"

int main(void) {
    // get kernel info
    FILE *f = fopen("/proc/kernel", "r");
    if(!f) {
        fprintf(stderr, "unable to open /proc/kernel for reading\n");
        return -1;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *kernel = calloc(1, size+1);
    if(!kernel) {
        fprintf(stderr, "failed to allocate memory\n");
        fclose(f);
        return -1;
    }

    if(fread(kernel, 1, size, f) != size) {
        fprintf(stderr, "failed to read from /proc/kernel\n");
        fclose(f);
        return -1;
    }

    printf("%s\n", kernel);
    return 0;
}