#include <stdio.h>
#include <stdlib.h>

#include "file.h"
#include "struct.h"

void openFile(FILE **fp, char *fn){
    *fp = fopen(fn, "r");
    if(*fp == NULL){
        exit(0);
    }
}

int readLine(FILE **fp, LINE **line){
    LINE *lineTemp;
    CODE *codeTemp, *ptr;
    static int num = 0;
    int r = -1;
    char c;

    lineTemp = (LINE*)malloc(sizeof(LINE));
    lineTemp -> head = NULL;
    lineTemp -> tail = NULL;
    lineTemp -> next = NULL;

    do{
        c = fgetc(*fp);
        codeTemp = (CODE *)malloc(sizeof(CODE));
        codeTemp -> c = c;
        codeTemp -> next = NULL;

        if(lineTemp -> head == NULL){
            lineTemp -> head = codeTemp;
            lineTemp -> tail = codeTemp;
        }
        else{
            lineTemp -> tail -> next = codeTemp;
            lineTemp -> tail = codeTemp;
        }
        r++;
    }while(c != EOF && c != '\n');

    lineTemp -> line = num;
    num++;

    *line = lineTemp;

    return r;
}
