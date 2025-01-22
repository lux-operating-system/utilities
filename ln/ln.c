/*
 * luxOS - a unix-like operating system
 * Omar Elghoul, 2024-25
 * 
 * ln: Implementation of the ln command
 */

#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

int force = 0, soft = 0;

static int usage(const char *name) {
    fprintf(stderr, "usage: %s [-fs] source_file target_file\n", name);
    fprintf(stderr, "       %s [-fs] source_file... target_dir\n", name);
    return EXIT_FAILURE;
}

int main(int argc, char **argv) {
    opterr = 0;
    int opt;
    while((opt = getopt(argc, argv, "fs")) != -1) {
        switch(opt) {
        case 'f': force = 1; break;
        case 's': soft = 1; break;
        case '?':
            fprintf(stderr, "%s: invalid option -- %c\n", argv[0], opt);
            return usage(argv[0]);
        }
    }

    struct stat st;
    const char *target = argv[argc-1];
    int sourceCount = argc - optind - 1;

    if(sourceCount < 1) return usage(argv[0]);
    else if(sourceCount == 1) {
        if(lstat(argv[optind], &st)) {
            perror(argv[optind]);
            return EXIT_FAILURE;
        }

        if(S_ISLNK(st.st_mode)) {
            return EXIT_SUCCESS;
        }

        if(force && (!access(target, F_OK))) {
            if(unlink(target)) {
                perror(argv[optind]);
                return EXIT_FAILURE;
            }
        }

        if(soft) {
            if(symlink(argv[optind], target)) {
                perror(argv[optind]);
                return EXIT_FAILURE;
            }
        } else {
            if(link(argv[optind], target)) {
                perror(argv[optind]);
                return EXIT_FAILURE;
            }
        }

        return EXIT_SUCCESS;
    }

    // for multiple source files
    char fileTarget[PATH_MAX];
    int errors = 0;
    for(int i = 0; i < sourceCount; i++) {
        char *lastPath = strrchr(argv[i+optind], '/');
        if(!lastPath) lastPath = argv[i+optind];
        sprintf(fileTarget, "%s/%s", target, lastPath);

        if(lstat(argv[i+optind], &st)) {
            perror(argv[i+optind]);
            errors++;
            continue;
        }

        if(S_ISLNK(st.st_mode)) continue;

        if(force && (!access(fileTarget, F_OK))) {
            if(unlink(fileTarget)) {
                perror(argv[i+optind]);
                errors++;
                continue;
            }
        }

        if(soft) {
            if(symlink(argv[i+optind], fileTarget)) {
                perror(argv[i+optind]);
                errors++;
            }
        } else {
            if(link(argv[i+optind], fileTarget)) {
                perror(argv[i+optind]);
                errors++;
            }
        }
    }

    return errors;
}