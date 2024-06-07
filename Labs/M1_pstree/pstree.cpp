#include "../include/headers/check.h"
#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_FILENAME_L 256
#define MAX_CONTENT_L 10000
#define MAX_ARGS_N 10
#define HASH_TABLE_SIZE 32768
#define MAX_CHILDREN_N 65536
#define MAX_LEADING_CHAR_N 1024

typedef struct PidInfo {
    char name[MAX_FILENAME_L];
    __pid_t pid;
    __pid_t ppid;
    __pid_t children[MAX_CHILDREN_N];
    int nchild;

} PidInfo;

typedef struct HashNode {
    int key;
    PidInfo *value;
    struct HashNode *next;
} HashNode;

HashNode *pidTable[HASH_TABLE_SIZE];

unsigned int hash(int key) {
    return key % HASH_TABLE_SIZE;
}

void addPidInfo(int pid, PidInfo *pidInfo) {
    int index = hash(pid);
    HashNode *newNode = (HashNode *)malloc(sizeof(HashNode));
    newNode->key = pid;
    newNode->value = pidInfo;
    newNode->next = pidTable[index];
    pidTable[index] = newNode;
}

PidInfo *getPidInfo(int pid) {
    unsigned int index = hash(pid);
    HashNode *currentNode = pidTable[index];
    while (currentNode != NULL) {
        if (currentNode->key == pid) {
            return currentNode->value;
        }
        currentNode = currentNode->next;
    }
    return NULL;
}

void freeMemory(void **p) {
    if (p != NULL && *p != NULL) {
        free(*p);
        *p = NULL;
    }
}

void removePidInfo(int pid) {
    unsigned int index = hash(pid);
    HashNode *currentNode = pidTable[index];
    HashNode *prevNode = NULL;
    while (currentNode != NULL) {
        if (currentNode->key == pid) {
            if (prevNode == NULL) {
                pidTable[index] = currentNode->next;
            } else {
                prevNode->next = currentNode->next;
            }
            freeMemory((void**)&currentNode->value);
            freeMemory((void**)&currentNode);
            return;
        }
        prevNode = currentNode;
        currentNode = currentNode->next;
    }
}

bool containsArg(const char *args, char arg) {
    for (int i = 0; i < strlen(args); i ++) {
        if (args[i] == arg) {
            return true;
        }
    }
    return false;
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
// using strcpy
const char *getContentField(const char *content, const char *field) {
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

void buildRelas() {
    for (int i = 0; i < HASH_TABLE_SIZE; i ++) {
        HashNode *current = pidTable[i];
        while (current != NULL) {
            PidInfo *pidInfo = current->value;
            int pid = pidInfo->pid;
            int ppid = pidInfo->ppid;
            if (ppid != 0) {
                pidInfo = getPidInfo(ppid);
                pidInfo->children[pidInfo->nchild ++] = pid;
            }
            current = current->next;
        }
    }
}

void fillPidInfos() {
    DIR *dirp = opendir("/proc");
    check(dirp);
    int pid = 0;
    struct dirent *entry;
    while ((entry = readdir(dirp)) != NULL) {
        if ((pid = atoi(entry->d_name)) == 0) {
            continue;
        }
        // printf("d_ino: %ld d_off: %ld d_type: %d d_reclen: %d d_name: %s\n", entry->d_ino, entry->d_off, entry->d_type, entry->d_reclen, entry->d_name);
        char procStatusFilePath[MAX_FILENAME_L] = "/proc/"; 
        strcat(procStatusFilePath, entry->d_name);
        strcat(procStatusFilePath, "/status");
        int fileFd = open(procStatusFilePath, O_RDONLY);
        check(fileFd != -1);
        const char *content = readFileContent(procStatusFilePath);
        PidInfo *newPidInfo = (PidInfo *)malloc(sizeof(PidInfo));
        strcpy(newPidInfo->name, getContentField(content, "Name"));
        newPidInfo->pid = pid;
        newPidInfo->ppid = atoi(getContentField(content, "PPid"));
        newPidInfo->nchild = 0; // init
        addPidInfo(pid, newPidInfo);
    }
    buildRelas();
}

void _printTree(PidInfo *pidInfo, const char *leadingStr, const char mode) {
    if (pidInfo == NULL) {
        printf("cannot get systemd.\n");
    }
    if (mode == 'p') {
        printf("%s(pid:%d)", pidInfo->name, pidInfo->pid);
    } else {
        printf("%s", pidInfo->name);
    }
    int nchild = pidInfo->nchild;
    if (nchild == 0) {
        printf("\n");
        return;
    }
    if (nchild == 1) printf("\u2500\u2500\u2500");
    else printf("\u2500\u252C\u2500");
    char nextLeadingStr[MAX_LEADING_CHAR_N];
    strcpy(nextLeadingStr, leadingStr);
    int start = strlen(nextLeadingStr), end = strlen(nextLeadingStr) + strlen(pidInfo->name);
    if (mode == 'p') {
        char spid[20];
        sprintf(spid, "%d", pidInfo->pid);
        end += strlen(spid) + 6;
    }
    for (int k = start; k < MAX_LEADING_CHAR_N && k < end; k ++) nextLeadingStr[k] = ' ';
    strcat(&nextLeadingStr[end], " \u2502 ");
    _printTree(getPidInfo(pidInfo->children[0]), nextLeadingStr, mode);

    for (int i = 1; i < nchild; i ++) {
        char _leadingStr[MAX_LEADING_CHAR_N];
        strcpy(_leadingStr, leadingStr);
        int start = strlen(_leadingStr), end = strlen(_leadingStr) + strlen(pidInfo->name);
        if (mode == 'p') {
            char spid[20];
            sprintf(spid, "%d", pidInfo->pid);
            end += strlen(spid) + 6;
        }
        for (int k = start; k < MAX_LEADING_CHAR_N && k < end; k ++) _leadingStr[k] = ' ';
        _leadingStr[end] = '\0'; // note
        printf("%s", _leadingStr);
        if (i == nchild - 1) {
            printf(" \u2514\u2500");
        } else {
            printf(" \u251C\u2500");
        }
        _printTree(getPidInfo(pidInfo->children[i]), nextLeadingStr, mode);
    }
}

void printTree(const char mode) {
    PidInfo *root = getPidInfo(1);
    _printTree(root, "", mode);
}

int main(int argc, char *argv[]) {
    char *args = getCmdOps(argc, argv);
    if (containsArg(args, 'v')) {
        printf("pstree v1.0\n");
        return 0;
    }
    fillPidInfos();
    if (containsArg(args, 'p')) {
        printTree('p');
    } else {
        printTree('n');
    }
    freeMemory((void**)&args);
    return 0;
}