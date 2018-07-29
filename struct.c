#include <stdlib.h>

#include "struct.h"

void initProg(PROGRAM *p){
    p -> length = 4;
    p -> value = 4;
    p -> line = p -> tail = NULL;
    p -> count = 0;
}

void initNote(NOTE *n){
    n -> val = -1;
    n -> next = NULL;
}
