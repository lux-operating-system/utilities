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
#include <signal.h>
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

    if(!argc) {
        free(argv);
        return 0;
    }

    argv = realloc(argv, (argc+1) * sizeof(char *));
    if(!argv) {
        fprintf(stderr, "unable to allocate memory for program arguments\n");
        return -1;
    }

    argv[argc] = NULL;  // null terminate

    // check for built-in commands
    if(!strcmp(argv[0], "exit")) {
        int status = 0;
        if(argc >= 2) status = atoi(argv[1]);
        fprintf(stderr, "exit");
        exit(status);
    } else if(!strcmp(argv[0], "cd")) {
        if(argc != 2) {
            // TODO: this should redirect to HOME someday
            fprintf(stderr, "usage: cd directory_name\n");
            return -1;
        }

        if(chdir(argv[1])) {
            fprintf(stderr, "lush: cannot cd into %s\n", argv[1]);
            return -1;
        }

        return 0;
    }

    pid_t pid = fork();
    if(pid < 0) {
        perror("lush");
        return -1;
    }

    if(!pid) {
        execvp(argv[0], argv);

        // if we're here then the program failed to execute
        if(errno == ENOENT)
            fprintf(stderr, "lush: command not found: %s\n", argv[0]);
        else
            perror("lush");

        exit(-1);
    } else {
        // parent
        int status;
        if(waitpid(pid, &status, 0) != pid) return -1;
        if(WIFEXITED(status)) return WEXITSTATUS(status);
        else return -1;
    }

    return 0;
}

int main() {
    signal(SIGINT, SIG_IGN);

    uid = getuid();

    if(!uid) {
        printf("logged in as root on %s\n\n", ttyname(0));
    } else {
        printf("logged in as uid %d on %s\n\n", uid, ttyname(0));
    }

    // set shell name
    setenv("SHELL", "lush", 1);

    char *line = NULL;
    size_t lineSize = 4096;     // arbitrary limit for now
    char promptColor = '2';     // green by default

    for(;;) {
        printf("\e[0;96m%s \e[0;9%cm%c\e[97m ", getwd(wd), promptColor, uid ? '$' : '#');
        fflush(stdout);
        if(line) free(line);

        line = NULL;
        lineSize = 4096;

        getline(&line, &lineSize, stdin);
        if(!line || !strlen(line)) continue;
        printf("\e[0m");
        fflush(stdout);
        if(execute(line)) promptColor = '1';    // red on error
        else promptColor = '2';     // green by default
    }

    return 0;
}
