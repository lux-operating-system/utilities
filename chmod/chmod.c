/*
 * luxOS - a unix-like operating system
 * Omar Elghoul, 2024
 * 
 * chmod: Implementation of the chmod command
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

static const mode_t bitmap[] = {
    S_IXOTH,
    S_IWOTH,
    S_IROTH,
    S_IXGRP,
    S_IWGRP,
    S_IRGRP,
    S_IXUSR,
    S_IWUSR,
    S_IRUSR
};

int parse(const char *modestr, mode_t *set, mode_t *clear) {
    *set = 0;
    *clear = 0;

    /* numerical mode setting */
    if(modestr[0] >= '0' && modestr[0] <= '9') {
        int num = atoi(modestr);
        
        for(int i = 0; i < 3; i++) {
            int digit = num % 10;
            num /= 10;
            if(digit > 7) return 1;

            if(digit & 1) {     // execute
                *set |= bitmap[i * 3];
            } else {
                *clear |= bitmap[i * 3];
            }

            if(digit & 2) {     // write
                *set |= bitmap[(i * 3) + 1];
            } else {
                *clear |= bitmap[(i * 3) + 1];
            }

            if(digit & 4) {     // read
                *set |= bitmap[(i * 3) + 2];
            } else {
                *clear |= bitmap[(i * 3) + 2];
            }
        }

        return 0;
    }

    /* TODO: symbolic mode setting */
    return 1;
}

int main(int argc, char **argv) {
    /* TODO: add support for -R to recursively change file modes */

    if(argc < 3) {
        fprintf(stderr, "usage: %s [-R] mode file...\n", argv[0]);
        return EXIT_FAILURE;
    }

    mode_t set = 0, clear = 0;
    if(parse(argv[1], &set, &clear)) {
        fprintf(stderr, "%s: invalid mode -- %s\n", argv[0], argv[1]);
        return EXIT_FAILURE;
    }

    int errors = 0;

    struct stat st;
    for(int i = 2; i < argc; i++) {
        if(stat(argv[i], &st)) {
            perror(argv[i]);
            errors++;
            continue;
        }

        st.st_mode &= ~clear;
        st.st_mode |= set;
        if(chmod(argv[i], st.st_mode)) {
            perror(argv[i]);
            errors++;
        }
    }

    return errors;
}
