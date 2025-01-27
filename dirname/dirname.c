/*
 * luxOS - a unix-like operating system
 * Omar Elghoul, 2024-25
 * 
 * dirname: Implementation of the dirname command
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

int main(int argc, char **argv) {
    if(argc < 2) {
        fprintf(stderr, "usage: %s string...\n", argv[0]);
        return EXIT_FAILURE;
    }

    char new[PATH_MAX];
    const char *old;

    for(int i = 1; i < argc; i++) {
        old = argv[i];
        memset(new, 0, PATH_MAX);

        if((strlen(old) == 1) && (old[0] == '/')) {
            new[0] = '/';
            goto print;
        }

        if((strlen(old) == 2) && (old[0] == '/') && (old[1] == '/')) {
            new[0] = '/';
            goto print;
        }

        strcpy(new, old);

        for(int j = strlen(new)-1; j >= 0; j--) {
            if(new[j] == '/') new[j] = 0;
            else break;
        }

        if(!strchr(new, '/')) {
            sprintf(new, ".");
            goto print;
        }

        for(int j = strlen(new)-1; j >= 0; j--) {
            if(new[j] != '/') new[j] = 0;
            else {
                new[j] = 0;
                break;
            }
        }

        if(!strlen(new)) sprintf(new, "/");

print:
        printf("%s\n", new);
    }

    return EXIT_SUCCESS;
}
