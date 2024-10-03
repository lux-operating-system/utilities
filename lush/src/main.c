/*
 * luxOS - a unix-like operating system
 * Omar Elghoul, 2024
 * 
 * lush: a minimalist Unix-like shell
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

uid_t uid;
char wd[PATH_MAX];

int execute(char *program) {
    int argc = 0;
    char **argv = NULL;
    char *token = strtok(program, " \n");
    while(token) {
        argv = realloc(argv, (argc+1) * sizeof(char *));
        if(!argv) {
            fprintf(stderr, "unable to allocate memory for program arguments\n");
            return -1;
        }

        argv[argc] = malloc(strlen(token) + 1);
        if(!argv[argc]) {
            fprintf(stderr, "unable to allocate memory for program arguments\n");
            return -1;
        }

        strcpy(argv[argc], token);

        token = strtok(NULL, " \n");
        argc++;
    }

    argv = realloc(argv, (argc+1) * sizeof(char *));
    if(!argv) {
        fprintf(stderr, "unable to allocate memory for program arguments\n");
        return -1;
    }

    argv[argc] = NULL;  // null terminate

    // check for built-in commands
    if(!strcmp(argv[0], "cd")) {
        if(chdir(argv[1])) {
            fprintf(stderr, "lush: cannot cd into %s\n", argv[1]);
            return -1;
        }

        return 0;
    } else if(!strcmp(argv[0], "pwd")) {
        printf("%s\n", getwd(wd));
        return 0;
    }

    pid_t pid = fork();
    if(pid < 0) {
        fprintf(stderr, "failed to fork() new program\n");
        return -1;
    }

    if(!pid) {
        execvp(argv[0], argv);

        // if we're here then the program failed to execute
        if(errno == ENOENT) {
            fprintf(stderr, "lush: command not found: %s\n", argv[0]);
        } else if(errno == ENOMEM) {
            fprintf(stderr, "lush: out of memory\n");
        } else {
            fprintf(stderr, "lush: failed to exec: errno = %d\n", errno);
        }

        exit(-1);
    } else {
        // parent
        int status;
        if(waitpid(pid, &status, 0) != pid) return -1;

        if(WIFEXITED(status)) return WEXITSTATUS(status);
    }

    return 0;
}

int main() {
    uid = getuid();

    printf("lux shell\n");
    if(!uid) {
        printf("logged in as root on %s\n\n", ttyname(0));
    } else {
        printf("logged in as uid %d\n\n", uid);
    }

    char *line = NULL;
    size_t lineSize = 4096;     // arbitrary limit for now

    for(;;) {
        printf("%s %c ", getwd(wd), uid ? '$' : '#');
        if(line) free(line);

        line = NULL;
        lineSize = 4096;

        getline(&line, &lineSize, stdin);
        execute(line);
    }

    return 0;
}
