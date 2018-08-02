#pragma once

#include <limits.h>
#define MIN INT_MIN

typedef struct code{
    char c;
    struct code *next;
} CODE;

typedef struct header{
    char clef;
    int keysig;
} HEADER;

typedef struct line{
    int line;
    HEADER *header;
    CODE *head, *tail;
    struct line *next;
} LINE;

typedef struct program{
    int value, length, count;
    LINE *line, *tail;
} PROGRAM;

typedef struct note{
    int val;
    struct note *next, *prev;
} NOTE;

typedef struct play{
    int playTime;
    NOTE *played;
    struct play *next;
} PLAY;

typedef struct list{
    int count;
    PLAY *head, *tail;
} PLAYLIST;

typedef struct loop{
    int count;
    CODE *c;
    struct loop *next;
} LOOP;

void initProg(PROGRAM*);
void initNote(NOTE*);
void initNoteToMin(NOTE*);
void initMarker(NOTE**);

void printLine(LINE*);
void printProgram(PROGRAM*);
void addLine(PROGRAM*, LINE*);
void addToLine(LINE*, LINE*);
