#pragma once

#include "struct.h"
#include "status.h"

LINE* getLine(PROGRAM*, char);
STATUS getHeader(PROGRAM*, LINE*, HEADER*);
STATUS getTimeSig(PROGRAM*, LINE*);

void play(PLAYLIST*, NOTE*, int, int);
void playAll(PLAYLIST*);
STATUS playProgram(PROGRAM*, PLAYLIST*);
