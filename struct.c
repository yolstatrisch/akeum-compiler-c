#include <stdio.h>
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

void initNoteToMin(NOTE *n){
    n = (NOTE*)malloc(sizeof(NOTE));
    n -> val = MIN;
    n -> next;
}

void initMarker(NOTE **n){
    int i;
    for(i = 0; i < 26; i++){
        n[i] = NULL;
    }
}

void printLine(LINE *line){
    CODE *ptr = line -> head;

    while(ptr -> next != NULL){
        printf("%c", ptr -> c);
        ptr = ptr -> next;
    }
    printf("\n");
}

void printProgram(PROGRAM *p){
    LINE *l = p -> line;
    CODE *c = l -> head;

    while(l != NULL){
        while(c != NULL){
            printf("%c", c -> c);
            c = c -> next;
        }
        l = l -> next;
        if(l != NULL){
            c = l -> head;
        }
    }
}

void addLine(PROGRAM *p, LINE *l){
    if(p -> line == NULL){
        p -> line = p -> tail = l;
    }
    else{
        p -> tail -> next = l;
        p -> tail = l;
    }
    p -> count++;
}

void addToLine(LINE *t, LINE *l){
    t -> tail -> next = l -> head;
    t -> tail = l -> tail;
    free(l);
}
