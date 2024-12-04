/*
 * luxOS - a unix-like operating system
 * Omar Elghoul, 2024
 * 
 * head: Implementation of the head command
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int n = 10;
int totalLines = 0;
char *line = NULL;

int head(FILE *input) {
    int errors = 0;

    if(line) free(line);
    line = NULL;
    size_t lineSize = 4096;

    while(totalLines < n) {
        getline(&line, &lineSize, input);
        if(!line) {
            errors++;
        } else {
            printf("%s", line);
            totalLines++;
        }

        if(feof(input)) break;
    }

    return errors;
}

int main(int argc, char **argv) {
    opterr = 0;
    int opt;

    while((opt = getopt(argc, argv, "n:")) != -1) {
        if(opt == 'n') {
            n = atoi(optarg);
            if(n < 1) {
                fprintf(stderr, "%s: invalid line count -- %d\n", argv[0], n);
                return -1;
            }
        } else if(opt == ':') {
            fprintf(stderr, "%s: option -%c requires an operand\n", argv[0], optopt);
            return -1;
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
            input = fopen(argv[i], "r");
            if(!input) {
                fprintf(stderr, "%s: cannot open %s for reading\n", argv[0], argv[i]);
                retval++;
                continue;
            }

            retval += head(input);
        }
    } else {
        input = stdin;
        retval += head(input);
    }

    return retval;
}