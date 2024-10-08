/*
 * luxOS - a unix-like operating system
 * Omar Elghoul, 2024
 * 
 * ls: Implementation of the ls command
 */

#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>

int h = 0, l = 0, a = 0;

char cwdbuf[PATH_MAX];

int statDump(char *name, char *path, struct stat *st) {
    if(!a && path[0] == '.') return 0;

    char color[8];
    char reset[] = "\e[0m";
    char buffer[PATH_MAX];

    if(S_ISCHR(st->st_mode))
        sprintf(color, "\e[0;93m");
    else if(S_ISDIR(st->st_mode))
        sprintf(color, "\e[0;96m");
    else if(S_ISBLK(st->st_mode))
        sprintf(color, "\e[0;91m");
    else if(S_ISLNK(st->st_mode))
        sprintf(color, "\e[0;95m");
    else if(st->st_mode & S_IXUSR || st->st_mode & S_IXGRP || st->st_mode & S_IXOTH)
        sprintf(color, "\e[0;92m");
    else
        sprintf(color, "%s", reset);

    if(l) {
        // detailed listing
        sprintf(buffer, "%s", reset);
        
        // file type
        if(S_ISCHR(st->st_mode)) sprintf(buffer+strlen(buffer), "c");
        else if(S_ISDIR(st->st_mode)) sprintf(buffer+strlen(buffer), "d");
        else if(S_ISBLK(st->st_mode)) sprintf(buffer+strlen(buffer), "b");
        else if(S_ISLNK(st->st_mode)) sprintf(buffer+strlen(buffer), "l");
        else sprintf(buffer+strlen(buffer), "-");

        // permissions
        if(st->st_mode & S_IRUSR) sprintf(buffer+strlen(buffer), "r");
        else sprintf(buffer+strlen(buffer), "-");

        if(st->st_mode & S_IWUSR) sprintf(buffer+strlen(buffer), "w");
        else sprintf(buffer+strlen(buffer), "-");

        if(st->st_mode & S_IXUSR) sprintf(buffer+strlen(buffer), "x");
        else sprintf(buffer+strlen(buffer), "-");

        if(st->st_mode & S_IRGRP) sprintf(buffer+strlen(buffer), "r");
        else sprintf(buffer+strlen(buffer), "-");

        if(st->st_mode & S_IWGRP) sprintf(buffer+strlen(buffer), "w");
        else sprintf(buffer+strlen(buffer), "-");

        if(st->st_mode & S_IXGRP) sprintf(buffer+strlen(buffer), "x");
        else sprintf(buffer+strlen(buffer), "-");

        if(st->st_mode & S_IROTH) sprintf(buffer+strlen(buffer), "r");
        else sprintf(buffer+strlen(buffer), "-");

        if(st->st_mode & S_IWOTH) sprintf(buffer+strlen(buffer), "w");
        else sprintf(buffer+strlen(buffer), "-");

        if(st->st_mode & S_IXOTH) sprintf(buffer+strlen(buffer), "x");
        else sprintf(buffer+strlen(buffer), "-");

        sprintf(buffer+strlen(buffer), "  ");

        // owner and group UIDs
        if(!st->st_uid) sprintf(buffer+strlen(buffer), "root  ");
        else sprintf(buffer+strlen(buffer), "%04X ", st->st_uid);

        if(!st->st_gid) sprintf(buffer+strlen(buffer), "root  ");
        else sprintf(buffer+strlen(buffer), "%04X ", st->st_gid);

        if(h) {
            // human-readable sizes (MB, GB, etc)
            if(st->st_size >= 0x40000000)
                sprintf(buffer+strlen(buffer), "%6dG  ", st->st_size / 0x40000000);
            else if(st->st_size >= 0x100000)
                sprintf(buffer+strlen(buffer), "%6dM  ", st->st_size / 0x100000);
            else if(st->st_size > 4096)
                sprintf(buffer+strlen(buffer), "%6dk  ", st->st_size / 1024);
            else
                sprintf(buffer+strlen(buffer), "%7d  ", st->st_size);
        } else {
            // default sizes (bytes)
            sprintf(buffer+strlen(buffer), "%9d  ", st->st_size);
        }

        sprintf(buffer+strlen(buffer), "%s%s%s\n", color, path, reset);
        printf("%s", buffer);
    } else {
        // undetailed
        printf("%s%s%s\n", color, path, reset);
    }

    return 0;
}

int dir(char *name, char *parent) {
    DIR *dir = opendir(parent);
    if(!dir) {
        fprintf(stderr, "%s: could not open directory %s\n", name, parent);
        return 1;
    }

    int errors = 0;
    char buffer[PATH_MAX];
    struct dirent *entry;
    struct stat st;

    while((entry = readdir(dir))) {
        sprintf(buffer, "%s/%s", parent, entry->d_name);
        if(stat(buffer, &st)) {
            fprintf(stderr, "%s: could not stat %s\n", name, buffer);
            errors++;
        } else {
            errors += statDump(name, entry->d_name, &st);
        }
    }

    closedir(dir);
    return errors;
}

int ls(char *name, char *path) {
    // determine if this is a file or directory
    struct stat st;
    if(stat(path, &st)) {
        fprintf(stderr, "%s: could not stat %s\n", name, path);
        return 1;
    }

    if(S_ISDIR(st.st_mode)) return dir(name, path);

    return statDump(name, path, &st);
}

int main(int argc, char **argv) {
    opterr = 0;
    int opt;
    while((opt = getopt(argc, argv, "hal")) != -1) {
        switch(opt) {
        case 'h': h = 1; break;
        case 'a': a = 1; break;
        case 'l': l = 1; break;
        case '?':
            fprintf(stderr, "%s: invalid option -- %c\n", argv[0], optopt);
            fprintf(stderr, "usage: %s [-hal] [file ...]\n", argv[0]);
            return -1;
        }
    }

    int retval = 0;

    if(optind < argc) {
        for(int i = optind; i < argc; i++) {
            retval += ls(argv[0], argv[i]);
        }
    } else {
        char *cwd = getcwd(cwdbuf, PATH_MAX);
        if(cwd) {
            retval = ls(argv[0], cwd);
        } else {
            fprintf(stderr, "%s: could not get current directory\n", argv[0]);
            retval = 1;
        }
    }

    return retval;
}