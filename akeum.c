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
#define OP_REST 51

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
    HEADER *header;
    CODE *head, *tail;
    struct line *next;
} LINE;

typedef struct program{
    int value, length, count;
    LINE *line, *tail;
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
void initProg(PROGRAM*);
void printProgram(PROGRAM*);
void addLine(PROGRAM*, LINE*);
void addToLine(LINE*, LINE*);

void printStatus(STATUS);

LINE* getLine(PROGRAM*, char);

STATUS getStatus(int, int, int);
STATUS getHeader(PROGRAM*, LINE*, HEADER*);
STATUS getTimeSig(PROGRAM*, LINE*);

STATUS programToQueue(PROGRAM*, QUEUE**);

void printProgram(PROGRAM *p){
    LINE *l = p -> line;
    CODE *c = l -> head;
    printf("Value: %d\nLength: %d\n", p -> value, p -> length);

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

void printStatus(STATUS stat){
    if(stat.status != SUCCESS){
        printf("ERROR: %d at line %d : %d\n", stat.status, stat.line, stat.c);
        exit(stat.status);
    }
}

int main(int argc, char *argv[]){
    FILE *fp;
    PROGRAM p;
    LINE *line, *temp;
    CODE *ptr;
    STATUS stat;
    HEADER *head;
    QUEUE **q;

    openFile(&fp, argv[1]);
    initProg(&p);

    while(readLine(&fp, &line)){
        printStatus(getHeader(&p, line, head));

        if(line -> line == 0){
            printStatus(getTimeSig(&p, line));
        }
        temp = getLine(&p, line -> header -> clef);
        if(temp != NULL){
            addToLine(temp, line);
        }
        else{
            addLine(&p, line);
        }
        //free(temp);
        //printLine(line);
    }
    //printProgram(&p);
    programToQueue(&p, q);

    fclose(fp);
    return 1;
}

void initOP(OP *op){
    op -> op = OP_NONE;
    op -> param = -1;
    op -> ptr = NULL;
    op -> next = NULL;
}

void addOP(QUEUE* q, OP *op){
    if(q -> head == NULL){
        q -> head = q -> tail = op;
    }
    else{
        q -> tail -> next = op;
        q -> tail = op;
    }
}

STATUS programToQueue(PROGRAM *p, QUEUE **q){
    LINE *l = p -> line;
    CODE *c = l -> head;
    q = (QUEUE**)malloc(p -> count * sizeof(QUEUE*));
    QUEUE *ptr;
    OP *temp;
    int cnt = 0;

    while(l != NULL){
        q[cnt] = (QUEUE*)malloc(sizeof(QUEUE));
        ptr = q[cnt];
        ptr -> head = ptr -> tail = NULL;

        while(c != NULL){
            switch(c -> c){
                case '|':
                    temp = (OP*)malloc(sizeof(OP));
                    initOP(temp);

                    if(c -> next -> c == '|' && c -> next -> next -> c == ':'){
                        temp -> op = OP_LOOP_S;
                    }
                    else{
                        temp -> op = OP_NONE;
                    }
                    addOP(ptr, temp);
                    break;
                case ':':
                    if(c -> next -> c == '|' && c -> next -> next -> c == ':'){
                        temp = (OP*)malloc(sizeof(OP));
                        initOP(temp);
                        temp -> op = OP_LOOP_S;
                        addOP(ptr, temp);
                    }
                    else{
                        //return wrong syntax for :||
                    }
                    break;
                case '+':
                    temp = (OP*)malloc(sizeof(OP));
                    initOP(temp);
                    temp -> op = OP_T_UP;
                    addOP(ptr, temp);
                    break;
                case '-':
                    temp = (OP*)malloc(sizeof(OP));
                    initOP(temp);
                    temp -> op = OP_T_DOWN;
                    addOP(ptr, temp);
                    break;
                case '.':
                    temp = (OP*)malloc(sizeof(OP));
                    initOP(temp);
                    temp -> op = OP_T_SET;
                    addOP(ptr, temp);
                    break;
                case '%':
                    temp = (OP*)malloc(sizeof(OP));
                    initOP(temp);
                    temp -> op = OP_REST;
                    addOP(ptr, temp);
                    break;
                case '~':
                    temp = (OP*)malloc(sizeof(OP));
                    initOP(temp);
                    temp -> op = OP_FORK;
                    addOP(ptr, temp);
                    break;
                case '=':
                    temp = (OP*)malloc(sizeof(OP));
                    initOP(temp);
                    temp -> op = OP_PLAYM;
                    addOP(ptr, temp);
                    break;
                case '?':
                    temp = (OP*)malloc(sizeof(OP));
                    initOP(temp);
                    temp -> op = OP_RAND;
                    addOP(ptr, temp);
                    break;
            }
            c = c -> next;
        }
        l = l -> next;
        if(l != NULL){
            c = l -> head;
        }
        cnt++;
    }
}

LINE* getLine(PROGRAM *p, char c){
    LINE *ptr = p -> line;

    while(ptr != NULL){
        if(ptr -> header -> clef == c){
            return ptr;
        }
        ptr = ptr -> next;
    }

    return NULL;
}

void initProg(PROGRAM *p){
    p -> length = 4;
    p -> value = 4;
    p -> line = p -> tail = NULL;
    p -> count = 0;
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

STATUS getHeader(PROGRAM *p, LINE *l, HEADER *h){
    CODE *ptr;
    int c = 0, s = 0;
    h = (HEADER*)malloc(sizeof(HEADER));
    h -> clef = 'G';
    h -> keysig = 0;

    ptr = l -> head;

    if(ptr -> c != '|'){
        return getStatus(ERR_HEAD_NONE, l -> line, c);
    }

    ptr = ptr -> next;
    c++;

    if(ptr -> c < 65 || ptr -> c > 90){
        return getStatus(ERR_HEAD_CLEF, l -> line, c);
    }
    else{
        h -> clef = ptr -> c;
    }

    ptr = ptr -> next;
    c++;

    while(ptr -> c == '#'){
        ptr = ptr -> next;
        s++;
        c++;
    }
    h -> keysig = s;
    l -> header = h;
    l -> head = ptr;

    return getStatus(SUCCESS, l -> line, 0);
}

STATUS getTimeSig(PROGRAM *p, LINE *l){
    CODE *ptr;
    int c = 0;

    ptr = l -> head;

    if(ptr -> next -> c == '-'){
        if(ptr -> c > 48 && ptr -> c < 58){
            p -> value = ptr -> c - 48;

            if(ptr -> next -> next -> c > 48 && ptr -> next -> next -> c < 58){
                p -> length = ptr -> next -> next -> c - 48;
            }
            else{
                return getStatus(ERR_HEAD_TIME_FORMAT, l -> line, c);
            }
        }
        else{
            return getStatus(ERR_HEAD_TIME_FORMAT, l -> line, c);
        }
    }
    else{
        return getStatus(ERR_HEAD_TIME_NONE, l -> line, c);
    }

    l -> head = ptr -> next -> next -> next;

    return getStatus(SUCCESS, l -> line, 0);
}

