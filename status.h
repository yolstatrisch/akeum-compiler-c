#pragma once

#include "struct.h"

#define SUCCESS 0
#define ERR_HEAD_NONE 1
#define ERR_HEAD_CLEF 2
#define ERR_HEAD_TIME_NONE 3
#define ERR_HEAD_TIME_FORMAT 4
#define ERR_INV_COMMAND 10
#define ERR_PLAY_LIST_EMPTY 20
#define ERR_PLAY_NULL 21
#define ERR_PLAY_OOB 22
#define ERR_UNK -1

typedef struct stat{
    int status, line, c;
} STATUS;

STATUS getStatus(int, int, int);
void printStatus(STATUS);
