#include "../include/headers/check.h"
#include <cctype>
#include <cstddef>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_FILENAME_L 256
#define MAX_PID_N 10000
#define MAX_CONTENT_L 10000
#define MAX_ARGS_N 10

typedef struct PidInfo {
    char name[MAX_FILENAME_L];
    __pid_t pid;
    __pid_t ppid;
} PidInfo;

PidInfo *pidInfos = nullptr;

bool containsArg(const char *args, char arg) {
    for (int i = 0; i < strlen(args); i ++) {
        if (args[i] == arg) {
            return true;
        }
    }
    return false;
}

void freeMemory(void **p) {
    if (p != NULL && *p != NULL) {
        free(*p);
        *p = NULL;
    }
}

char *getCmdOps(int argc, char *argv[]) {
    check(argc >= 1);
    int argChs = 0;
    for (int i = 1; i < argc; i ++) {
        argChs += strlen(argv[i]) - 1;
    }
    if (argChs > MAX_ARGS_N) {
        printf("The number of arguments should be less than %d\n", MAX_ARGS_N);
        exit(1);
    }
    char *args = (char *)malloc(sizeof(char) * MAX_ARGS_N + 10);
    check(args);
    for (int i = 1; i < argc; i ++) {
        if (argv[i][0] != '-') {
            printf("The NO.%d argument should begin with \'-\'\n", i);
            exit(1);
        }
        strcat(args, argv[i] + 1);
    }
    return args;
}

char *readFileContent(char *path) {
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        printf("cannot open the file \'%s\\n'", path);
        return NULL;
    }

    size_t bufferSize = 4096;
    size_t haveRead = 0;
    char *buffer = (char *)malloc(bufferSize);
    ssize_t rbytes;
    while((rbytes = read(fd, buffer + haveRead, bufferSize - haveRead - 1)) > 0) {
        haveRead += rbytes;
        if (haveRead >= bufferSize - 1) {
            bufferSize *= 2;
            char *newBuffer = (char *)realloc(buffer, bufferSize);
            check(newBuffer);
            buffer = newBuffer;
        }
    }
    if (rbytes == -1) {
        printf("cannot read the file \'%s\'\n", path);
        freeMemory((void**)&buffer);
        close(fd);
        return NULL;
    }

    buffer[haveRead] = '\0';
    close(fd);
    return buffer;
}

// no need for free, no more than 16k.
const char *getValue(const char *content, const char *field) {
    static char buffer[16384];
    const char *start, *end;
    start = strstr(content, field);
    if (start == NULL) return NULL;
    if (start[strlen(field)] != ':' && !isspace(start[strlen(field)])) return NULL;
    start = strstr(start, ":");
    if (start == NULL) return NULL;
    start ++;
    while(isspace(*start)) {
        if (*start == '\n') return "";
        start ++;
    }
    end = strstr(start, "\n");
    if (end == NULL) {
        end = strchr(start, '\0');
    }

    size_t length = end - start;
    if (end - start >= sizeof(buffer)) {
        length = sizeof(buffer) - 1;
    }
    strncpy(buffer, start, length);
    buffer[length] = '\0';
    return buffer;
}

void setPidInfos() {
    check(pidInfos);
    DIR *dirp = opendir("/proc");
    check(dirp);
    int pid = 0, pids_count = 0;
    struct dirent *entry;
    while ((entry = readdir(dirp)) != NULL) {
        if ((pid = atoi(entry->d_name)) == 0) {
            continue;
        }
        pidInfos[pids_count].pid = pid;

        char procStatusFilePath[MAX_FILENAME_L] = "/proc/"; 
        strcat(procStatusFilePath, entry->d_name);
        strcat(procStatusFilePath, "/status");
        int fileFd = open(procStatusFilePath, O_RDONLY);
        check(fileFd != -1);
        const char *content = readFileContent(procStatusFilePath);
        printf("Read from file \'%s\'\n", procStatusFilePath);
        printf("d_ino: %ld d_off: %ld d_type: %d d_reclen: %d d_name: %s\n",\
            entry->d_ino, entry->d_off, entry->d_type, entry->d_reclen, entry->d_name);
        // printf("Content: \n%s\n", content);
        printf("Name: %s\n", getValue(content, "Name"));
        pids_count ++;
    }
}

int main(int argc, char *argv[]) {
    pidInfos = (PidInfo *)malloc(sizeof(PidInfo) * MAX_PID_N);
    check(pidInfos);
    char *args = getCmdOps(argc, argv);
    if (containsArg(args, 'v')) {
        printf("pstree v1.0\n");
    }
    if (containsArg(args, 'p')) {
        setPidInfos();
    }

    freeMemory((void**)&pidInfos);
    freeMemory((void**)&args);
    return 0;
}