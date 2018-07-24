#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OP_NONE 0
#define OP_PLAYK 1
#define OP_PLAYM 2
#define OP_PLAYR 3
#define OP_FORK 10
#define OP_MARK 11
#define OP_T_UP 20
#define OP_T_DOWN 21
#define OP_T_SET 23
#define OP_LOOP_S 30
#define OP_LOOP_E 31
#define OP_RAND 50

#define SUCCESS 0
#define ERR_HEAD_NONE 1
#define ERR_HEAD_CLEF 2
#define ERR_HEAD_TIME_NONE 3
#define ERR_HEAD_TIME_FORMAT 4
#define ERR_UNK -1

typedef struct code{
    char c;
    struct code *next;
} CODE;

typedef struct header{
    char clef;
    int keysig;
} HEADER;

typedef struct line{
    HEADER header;
    CODE *head, *tail;
    struct line *next;
} LINE;

typedef struct program{
    int value, length;
    LINE *line;
} PROGRAM;

void openFile(FILE**, char*);
int readLine(FILE**, LINE**);

void printLine(LINE*);

int main(int argc, char *argv[]){
    FILE *fp;
    PROGRAM p;
    LINE *line;
    CODE *ptr;

    openFile(&fp, argv[1]);

    while(readLine(&fp, &line)){
        printLine(line);
    }

    fclose(fp);
    return 1;
}

void openFile(FILE **fp, char *fn){
    *fp = fopen(fn, "r");
    if(*fp == NULL){
        exit(0);
    }
}

int readLine(FILE **fp, LINE **line){
    LINE *lineTemp;
    CODE *codeTemp, *ptr;
    int r = -1;
    char c;

    lineTemp = (LINE*)malloc(sizeof(LINE));
    lineTemp -> head = NULL;
    lineTemp -> tail = NULL;

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

    *line = lineTemp;

    return r;
}

void printLine(LINE *line){
    CODE *ptr = line -> head;

    while(ptr -> next != NULL){
        printf("%c", ptr -> c);
        ptr = ptr -> next;
    }
    printf("\n");
}
