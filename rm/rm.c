/*
 * luxOS - a unix-like operating system
 * Omar Elghoul, 2024
 * 
 * rm: implementation of the rm command
 */

#include <stdio.h>
#include <ctype.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

int force = 0, interactive = 0, recursive = 0;
int terminal;   // whether stdin is a terminal

int usage(const char *name) {
    fprintf(stderr, "usage: %s [-fiRr] file...\n", name);
    return EXIT_FAILURE;
}

int rm(const char *file) {
    struct stat st;
    if(lstat(file, &st)) {
        if(!force) {
            perror(file);
            return 1;
        }
        return 0;
    }

    if(S_ISDIR(st.st_mode)) {
        if(!recursive) {
            fprintf(stderr, "%s: is a directory\n", file);
            return 1;
        }

        if(!force && (interactive || terminal)) {
            fprintf(stderr, "remove %s? ", file);
            int c = getchar();
            if(tolower(c) != 'y') return 0;
        }

        int errors = 0;
        DIR *dir = opendir(file);
        if(!dir) {
            perror(file);
            return 1;
        }

        struct dirent *dirent;
        while((dirent = readdir(dir))) {
            if((!strcmp(dirent->d_name, ".")) || (!strcmp(dirent->d_name, "..")))
                continue;

            char *path = malloc(PATH_MAX+1);
            if(!path) {
                perror(file);
                return errors + 1;
            }

            sprintf(path, "%s/%s", file, dirent->d_name);
            errors += rm(path);
            free(path);
        }

        if(closedir(dir)) {
            perror(file);
            errors++;
        }

        if(!errors) {
            if(rmdir(file)) {
                perror(file);
                errors++;
            }
        }

        return errors;
    }

    mode_t write = 0;
    if(getuid() == st.st_uid) write = st.st_mode & S_IWUSR;
    else if(getgid() == st.st_gid) write = st.st_mode & S_IWGRP;
    else write = st.st_mode & S_IWOTH;

    if(!force && (interactive || (terminal && !write))) {
        fprintf(stderr, "remove %s? ", file);
        int c = getchar();
        if(tolower(c) != 'y') return 0;
    }

    if(unlink(file)) return 1;
    return 0;
}

int main(int argc, char **argv) {
    if(argc < 2) return usage(argv[0]);

    opterr = 0;
    int opt;
    while((opt = getopt(argc, argv, "fiRr")) != -1) {
        switch(opt) {
        case 'f': force = 1; break;
        case 'i': interactive = 1; break;
        case 'r':
        case 'R': recursive = 1; break;
        case '?':
            fprintf(stderr, "%s: invalid option -- %c\n", argv[0], opt);
            return usage(argv[0]);
        }
    }

    if(optind >= argc) return usage(argv[0]);

    terminal = isatty(fileno(stdin));
    int errors = 0;
    for(int i = optind; i < argc; i++) {
        errors += rm(argv[i]);
    }

    return errors;
}
