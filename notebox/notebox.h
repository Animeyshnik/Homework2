#ifndef NOTEBOX_H
#define NOTEBOX_H


#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#define SHM_NAME "/notebox_shm"
#define MAX_NOTES 20
#define MAX_TEXT 200


typedef struct {
    char author[32];
    char timestamp[32];
    char text[MAX_TEXT];
    int active; // 1 if filled, 0 if empty
} Note;

typedef struct {
    Note notes[MAX_NOTES];
    pthread_mutex_t mutex;
    int user_count;
} NoteBox;

void get_timestamp(char* buffer, size_t size);

#endif
