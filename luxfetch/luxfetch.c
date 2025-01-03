/*
 * luxOS - a unix-like operating system
 * Omar Elghoul, 2024
 * 
 * luxfetch: neofetch clone for luxOS
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/lux/lux.h>

#define COLOR       "\e[0;36m"
#define RESET       "\e[0m"

#define LFB_GET_WIDTH           (0x10 | IOCTL_OUT_PARAM)
#define LFB_GET_HEIGHT          (0x20 | IOCTL_OUT_PARAM)

char colors[77];

char *colorTest(int bright) {
    memset(colors, 0, 77);

    for(int i = 0; i < 8; i++) {
        if(bright) sprintf(colors+strlen(colors), "\e[10%dm   ", i);
        else sprintf(colors+strlen(colors), "\e[4%dm   ", i);
    }

    sprintf(colors+strlen(colors), "\e[0m");
    return colors;
}

int main(void) {
    // get kernel info
    FILE *f = fopen("/proc/kernel", "r");
    if(!f) {
        fprintf(stderr, "unable to open /proc/kernel for reading\n");
        return -1;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *kernel = calloc(1, size+1);
    if(!kernel) {
        fprintf(stderr, "failed to allocate memory\n");
        fclose(f);
        return -1;
    }

    if(fread(kernel, 1, size, f) != size) {
        fprintf(stderr, "failed to read from /proc/kernel\n");
        fclose(f);
        return -1;
    }

    fclose(f);

    // get CPU model
    f = fopen("/proc/cpu", "r");
    if(!f) {
        fprintf(stderr, "unable to open /proc/cpu for reading\n");
        free(kernel);
        return -1;
    }

    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *cpu = calloc(1, size+1);
    if(!cpu) {
        fprintf(stderr, "failed to allocate memory\n");
        free(kernel);
        fclose(f);
        return -1;
    }

    if(fread(cpu, 1, size, f) != size) {
        fprintf(stderr, "failed to read from /proc/cpu\n");
        free(kernel);
        free(cpu);
        fclose(f);
        return -1;
    }

    fclose(f);

    // read uptime
    f = fopen("/proc/uptime", "r");
    if(!f) {
        fprintf(stderr, "unable to open /proc/uptime for reading\n");
        free(kernel);
        free(cpu);
        return -1;
    }

    uint64_t uptime = 0;
    fread(&uptime, 1, 8, f);

    uptime /= 1000;     // convert to seconds

    // read memory usage
    uint32_t memusage = 0, memsize = 0;
    f = fopen("/proc/memsize", "r");
    if(!f) {
        fprintf(stderr, "unable to open /proc/memsize for reading\n");
        free(kernel);
        free(cpu);
        return -1;
    }

    fread(&memsize, 1, 4, f);
    fclose(f);

    f = fopen("/proc/memusage", "r");
    if(!f) {
        fprintf(stderr, "unable to open /proc/memusage for reading\n");
        free(kernel);
        free(cpu);
        return -1;
    }

    fread(&memusage, 1, 4, f);
    fclose(f);

    // get screen resolution
    unsigned long width, height;
    int fd = open("/dev/lfb0", O_RDONLY);
    if(fd < 0) {
        free(kernel);
        free(cpu);
        fprintf(stderr, "unable to open /dev/lfb0 for reading\n");
        return -1;
    }

    if(ioctl(fd, LFB_GET_WIDTH, &width)) {
        close(fd);
        free(kernel);
        free(cpu);
        fprintf(stderr, "unable to get screen resolution\n");
        return -1;
    }

    if(ioctl(fd, LFB_GET_HEIGHT, &height)) {
        close(fd);
        free(kernel);
        free(cpu);
        fprintf(stderr, "unable to get screen resolution\n");
        return -1;
    }

    close(fd);

    // more readability
    int hours = (uptime / 3600);
    int mins = (uptime / 60) % 60;
    int secs = (uptime % 60);

    int usageMB = memusage / 256;
    int sizeMB = memsize / 256;

    printf(COLOR "          @@@@@@@@          %s" RESET "@" COLOR "localhost" RESET "\n", "root");
    printf(COLOR "        @@@      @@@        " RESET "--------------\n");
    printf(COLOR "      @@            @@      OS: " RESET "luxOS development\n");
    printf(COLOR "     @@              @@     Kernel: " RESET "%s\n", kernel);
    printf(COLOR "     @@              @@     Uptime: " RESET);

    if(hours) {
        printf("%d hours, %d mins, %d mins\n", hours, mins, secs);
    } else if(mins) {
        printf("%d mins, %d secs\n", mins, secs);
    } else {
        printf("%d secs\n", secs);
    }

    printf(COLOR "     @@              @@     Shell: " RESET "%s\n", getenv("SHELL"));
    printf(COLOR "     @@              @@     Resolution: " RESET "%dx%d\n", (int) width, (int) height);
    printf(COLOR "      @@            @@      Terminal: " RESET "%s\n", ttyname(0));
    printf(COLOR "       @@@        @@@       CPU: " RESET "%s\n", cpu);
    printf(COLOR "         @@@    @@@         Memory: " RESET "%d MiB / %d MiB\n", usageMB, sizeMB);
    printf(COLOR "          @@@@@@@@          " RESET "\n");
    printf(COLOR "                            " RESET "%s\n", colorTest(0));
    printf(COLOR "          @@@@@@@@          " RESET "%s\n", colorTest(1));
    printf("\n");

    return 0;
}