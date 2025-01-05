/*
 * luxOS - a unix-like operating system
 * Omar Elghoul, 2024
 * 
 * touch: Implementation of the touch command
 */

#include <stdio.h>
#include <utime.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char **argv) {
    /* TODO: parse args */
    if(argc < 2) {
        fprintf(stderr, "usage: %s [-acm] [-r ref_file|-t time|-d date_time] file...", argv[0]);
        return EXIT_FAILURE;
    }

    int errors = 0;

    for(int i = 1; i < argc; i++) {
        if(utime(argv[i], NULL)) {
            if(errno == ENOENT) {
                int fd = open(argv[i], O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
                if(fd < 0) {
                    perror(argv[i]);
                    errors++;
                } else {
                    close(fd);
                }
            } else {
                perror(argv[i]);
                errors++;
            }
        }
    }

    return errors;
}