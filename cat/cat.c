/*
 * luxOS - a unix-like operating system
 * Omar Elghoul, 2024
 * 
 * cat: Implementation of the cat command
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define DEFAULT_BUFFER          4096

int u = 0;
char buffer[DEFAULT_BUFFER+1];
size_t bufferCount = 0;

int cat(FILE *input) {
    bufferCount = 0;

    int errors = 0;
    int value;

    while(!feof(input)) {
        if(!u && (input != stdin)) {
            bufferCount = fread(buffer, 1, DEFAULT_BUFFER, input);
            if(bufferCount == 0) errors++;
            else if(fwrite(buffer, 1, bufferCount, stdout) != bufferCount) errors++;

            bufferCount = 0;
        } else {
            value = fgetc(input);
            if(value == EOF) errors++;
            else fputc(value, stdout);
        }
    }

    if(!u && bufferCount) {
        // write what's remaining of the buffer if EOF is encountered
        if(fwrite(buffer, 1, bufferCount, stdout) != bufferCount) errors++;
    }

    return errors;
}

int main(int argc, char **argv) {
    opterr = 0;
    int opt;

    while((opt = getopt(argc, argv, "u")) != -1) {
        if(opt == 'u') {
            u = 1;
        } else {
            fprintf(stderr, "%s: invalid option -- %c\n", argv[0], optopt);
            fprintf(stderr, "usage: %s [-u] [file ...]\n", argv[0]);
            return -1;
        }
    }

    int retval = 0;
    FILE *input;

    if(optind < argc) {
        for(int i = optind; i < argc; i++) {
            if(!strcmp(argv[i], "-")) {
                input = stdin;
            } else {
                input = fopen(argv[i], "r");
                if(!input) {
                    fprintf(stderr, "%s: cannot open %s for reading\n", argv[0], argv[i]);
                    retval++;
                    continue;
                }
            }

            retval += cat(input);
        }
    } else {
        input = stdin;      // default
        retval += cat(input);
    }

    return retval;
}