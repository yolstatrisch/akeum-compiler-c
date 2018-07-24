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
    int line;
    HEADER header;
    CODE *head, *tail;
    struct line *next;
} LINE;

typedef struct program{
    int value, length;
    LINE *line;
} PROGRAM;

typedef struct operation{
    int op;
    int param;
    void *ptr;
    struct operation *next;
} OP;

typedef struct queue{
    OP *head, *tail;
} QUEUE;

typedef struct stat{
    int status, line, c;
} STATUS;

void openFile(FILE**, char*);
int readLine(FILE**, LINE**);
void printLine(LINE*);
void printProgram(PROGRAM*);

void printStatus(STATUS);

STATUS getStatus(int, int, int);
STATUS getHeader(PROGRAM*, LINE*);
STATUS getTimeSig(PROGRAM*, LINE*);

void printProgram(PROGRAM *p){
    printf("Value: %d\nLength: %d\n", p -> value, p -> length);
}

void printStatus(STATUS stat){
    if(stat.status != SUCCESS){
        printf("ERROR: %d at line %d : %d\n", stat.status, stat.line, stat.c);
        exit(stat.status);
    }
}

int main(int argc, char *argv[]){
    FILE *fp;
    PROGRAM p;
    LINE *line;
    CODE *ptr;
    STATUS stat;

    openFile(&fp, argv[1]);

    while(readLine(&fp, &line)){
        if(line -> line == 0){
            stat = getHeader(&p, line);
            printStatus(stat);
        }
        printLine(line);
    }

    printProgram(&p);

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
    static int num = 0;
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

    lineTemp -> line = num;
    num++;

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

STATUS getStatus(int s, int l, int c){
    STATUS st;

    st.status = s;
    st.line = l;
    st.c = c;

    return st;
}

STATUS getHeader(PROGRAM *p, LINE *l){
    CODE *ptr;
    int c = 0, s = 0;

    ptr = l -> head;

    if(ptr -> c != '|'){
        return getStatus(ERR_HEAD_NONE, l -> line, c);
    }
    ptr = ptr -> next;
    c++;
    if(ptr -> c < 65 || ptr -> c > 90){
        return getStatus(ERR_HEAD_CLEF, l -> line, c);
    }
    ptr = ptr -> next;
    c++;
    while(ptr -> c == '#'){
        ptr = ptr -> next;
        s++;
        c++;
    }
    return getTimeSig(p, l);
}

STATUS getTimeSig(PROGRAM *p, LINE *l){
    CODE *ptr;
    int c = 0;

    p -> value = 4;
    p -> length = 4;
    ptr = l -> head;

    while(ptr -> next != NULL){
        if(ptr -> next -> c == '-'){
            if(ptr -> c > 48 && ptr -> c < 58){
                p -> value = ptr -> c - 48;

                if(ptr -> next -> next -> c > 48 && ptr -> next -> next -> c < 58){
                    p -> length = ptr -> next -> next -> c - 48;
                    break;
                }
                else{
                    return getStatus(ERR_HEAD_TIME_FORMAT, l -> line, c);
                }
            }
            else{
                return getStatus(ERR_HEAD_TIME_FORMAT, l -> line, c);
            }
        }
        c++;
        ptr = ptr -> next;
    }
    if(ptr -> next == NULL){
        return getStatus(ERR_HEAD_TIME_NONE, l -> line, c);
    }

    return getStatus(SUCCESS, l -> line, 0);
}
