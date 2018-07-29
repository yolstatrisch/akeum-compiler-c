#pragma once

#include "struct.h"
#include "status.h"

#define MIN INT_MIN

void initMarker(NOTE**);

void printLine(LINE*);
void printProgram(PROGRAM*);
void addLine(PROGRAM*, LINE*);
void addToLine(LINE*, LINE*);

LINE* getLine(PROGRAM*, char);
STATUS getHeader(PROGRAM*, LINE*, HEADER*);
STATUS getTimeSig(PROGRAM*, LINE*);

void play(PLAY*, NOTE*);
STATUS playProgram(PROGRAM*, PLAY**);
