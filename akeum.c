#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
#define ERR_INV_COMMAND 10
#define ERR_PLAY_LIST_EMPTY 20
#define ERR_PLAY_NULL 21
#define ERR_PLAY_OOB 22
#define ERR_UNK -1

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

typedef struct stat{
    int status, line, c;
} STATUS;

typedef struct note{
    int val;
    struct note *next, *prev;
} NOTE;

typedef struct play{
    int cnt;
    NOTE *head, *tail;
} PLAY;

typedef struct loop{
    int count;
    CODE *c;
    struct loop *next;
} LOOP;

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

STATUS playProgram(PROGRAM*, PLAY**);

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
    PLAY **pl;

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
    printStatus(playProgram(&p, pl));

    fclose(fp);
    return 1;
}

void initNote(NOTE *n){
    n -> val = -1;
    n -> next = NULL;
}

void play(PLAY* pl, NOTE *n){
    if(pl -> head == NULL){
        n -> prev = NULL;
        pl -> head = pl -> tail = n;
    }
    else{
        n -> prev = pl -> tail;
        pl -> tail -> next = n;
        pl -> tail = n;
    }
    pl -> cnt++;
    printf("Playing: %d\n", n -> val);
}

void initMarker(NOTE **n){
    int i;
    for(i = 0; i < 26; i++){
        n[i] = NULL;
    }
}

STATUS playProgram(PROGRAM *p, PLAY **pl){
    LINE *l = p -> line;
    CODE *c = l -> head;
    pl = (PLAY**)malloc(p -> count * sizeof(PLAY*));
    PLAY *ptr;
    NOTE *temp, *last;
    int cnt = 0;
    int value = p -> value;
    int length = p -> length;
    int tempval = 0;
    int mode;
    NOTE *marker[26];
    LOOP *loop;
    NOTE *add = NULL;
    CODE *inloop = NULL, *outloop = NULL;
    int r[] = {-9, -8, -7, -6, -5, -4, -3, -2, -1, 0, 1, 2};
    int i;

    initMarker(marker);
    srand(time(NULL));

    while(l != NULL){
        loop = NULL;
        last = (NOTE*)malloc(sizeof(NOTE));
        last -> val = MIN;
        last -> next;
        mode = 0;
        pl[cnt] = (PLAY*)malloc(sizeof(PLAY));
        ptr = pl[cnt];
        ptr -> head = ptr -> tail = last;
        ptr -> cnt = 0;

        while(c != NULL){
            int s = 12;
            if(isupper(c -> c)){
                tempval = (c -> c - 60) % 7;
                tempval = (tempval > 2) ? tempval * 2 - 10 : tempval * 2 - 9;
                temp = (NOTE*)malloc(sizeof(NOTE));
                if(c -> next -> c == '#'){
                    tempval++;
                    c = c -> next;
                }
                else if(c -> next -> c == 'b'){
                    tempval--;
                    c = c -> next;
                }
                tempval += value;
                tempval += mode;
                //printf("%c%c- %d\n", c -> c, (c -> next -> c == '#' || c -> next -> c == 'b') ? c -> next -> c : ' ', tempval);
                temp -> val = tempval;
                temp -> next = NULL;
                last = temp;
                play(ptr, temp);
            }
            else if(islower(c -> c)){
                tempval = c -> c - 97;
                //printf("%c - %c\n", c -> next -> next -> c, c -> c);
                if(c -> next -> c != '=' || c -> next -> next -> c != c -> c){
                    marker[tempval] = last;
                    //printf("Added marker %c at %d with val %d\n", c -> c, last, last -> val);
                }
                else{
                    add = last;
                    //printf("Adding marker %c at %d with val %d\n", c -> c, last, last -> val);
                }
            }
            switch(c -> c){
                case '|':
                    if(c -> next -> c == '|' && c -> next -> next -> c == ':'){
                        if(last -> val > 0 || last -> val == MIN){
                            LOOP *temp = (LOOP*)malloc(sizeof(LOOP));
                            temp -> count = last -> val - 1;
                            temp -> c = c -> next -> next;
                            temp -> next = NULL;

                            if(loop == NULL){
                                loop = temp;
                            }
                            else{
                                temp -> next = loop;
                                loop = temp;
                            }
                        }
                        else{
                            //printf("Skipping...\n");
                            int e = 1;
                            while(e){
                                //printf("%c%c%c\n", c -> c, c -> next -> c, c -> next -> next -> c);
                                if(c -> c == ':' && c -> next -> c == '|' && c -> next -> next -> c == '|'){
                                    e = 0;
                                }
                                //printf("pass\n");
                                c = c -> next;
                            }
                        }
                        c = c -> next -> next;
                    }
                    break;
                case ':':
                    if(c -> next -> c == '|' && c -> next -> next -> c == '|'){
                        if(loop -> count > 0){
                            loop -> count--;
                            c = loop -> c;
                        }
                        else if(loop -> count == MIN){
                            c = loop -> c;
                        }
                        else{
                            if(loop -> next != NULL){
                                LOOP *temp = loop;
                                loop = loop -> next;
                                free(temp);
                            }

                            c = c -> next -> next;
                        }
                    }
                    else{
                        return getStatus(ERR_INV_COMMAND, 0, 0);
                    }
                    break;
                case '+':
                    if(ptr -> tail -> val != MIN){
                        mode += ptr -> tail -> val;
                    }
                    else{
                        return getStatus(ERR_PLAY_NULL, 0, 0);
                    }
                    break;
                case '-':
                    if(ptr -> tail -> val != MIN){
                        mode -= ptr -> tail -> val;
                    }
                    else{
                        return getStatus(ERR_PLAY_NULL, 0, 0);
                    }
                    break;
                case '.':
                    mode = 0;
                    break;
                case '%':
                    temp = (NOTE*)malloc(sizeof(NOTE));
                    temp -> val = MIN;
                    temp -> next = NULL;

                    play(pl[cnt], temp);
                    last = temp;
                    break;
                case '~':
                    if(last -> val == 0){
                        //printf("Skip by fork\n");
                        int e = 1;
                            while(e || c -> next == NULL){
                                if(c -> c == ':' && c -> next -> c == '|' && c -> next -> next -> c == '|'){
                                    e = 0;
                                }
                                c = c -> next;
                            }
                    }
                    break;
                case '=':
                    if(islower(c -> next -> c)){
                        tempval = c -> next -> c - 97;
                        //printf("%d\n", marker[tempval] -> val);
                        //printf("Accessing marker %c at %d\n", c -> next -> c, marker[tempval]);
                        if(marker[tempval] -> next != NULL){
                            int tempplay = marker[tempval] -> next -> val;
                            tempplay += value;
                            tempplay += mode;
                            temp = (NOTE*)malloc(sizeof(NOTE));
                            temp -> val = tempplay;
                            temp -> next = NULL;

                            play(pl[cnt], temp);
                            last = temp;

                            if(add != NULL){
                                //printf("%d to %d\n", marker[tempval], last);
                                marker[tempval] = add;
                                add = NULL;
                            }
                        }
                        else{
                            return getStatus(ERR_PLAY_LIST_EMPTY, 0, 0);
                        }
                        c = c -> next;
                    }
                    else if(isdigit(c -> next -> c) || c -> next -> c == '-'){
                        tempval = pl[cnt] -> cnt;
                        int rel = 0, sign = 1, i;
                        if(c -> next -> c == '-'){
                            sign *= -1;
                        }
                        else{
                            rel = c -> next -> c - '0';
                        }

                        CODE *tempptr = c -> next -> next;
                        while(isdigit(tempptr -> c)){
                            rel = rel * 10 + ((tempptr -> c - '0') * sign);
                            c = tempptr;
                            tempptr = tempptr -> next;
                        }

                        if(abs(rel) > tempval){
                            return getStatus(ERR_PLAY_OOB, 0, 0);
                        }
                        //TODO: check for size first (optimization)
                        //printf("%d\n", rel);
                        NOTE *plptr;
                        if(rel >= 0){
                            plptr = pl[cnt] -> head;
                            while(rel > 0){
                                plptr = plptr -> next;
                                rel--;
                            }
                        }
                        else{
                            plptr = pl[cnt] -> tail;
                            rel++;
                            while(rel < 0){
                                plptr = plptr -> prev;
                                rel++;
                            }
                        }

                        plptr -> val += value;
                        plptr -> val += mode;

                        temp = (NOTE*)malloc(sizeof(NOTE));
                        temp -> val = plptr -> val;
                        temp -> next = NULL;

                        play(pl[cnt], temp);
                        last = temp;
                        //printf("%d\n", plptr -> val);
                        c = c -> next;
                    }
                    break;
                case '?':
                    while(s > 0){
                        temp = (NOTE*)malloc(sizeof(NOTE));
                        do{
                            i = rand() % 12;
                        }while(r[i] == MIN);

                        //printf("%d ",r[i]);
                        temp -> val = r[i] + value + mode;
                        temp -> next = NULL;
                        last = temp;
                        play(ptr, temp);

                        r[i] = MIN;
                        s--;
                    }
                    break;
            }
            c = c -> next;
            //printf("Mode: %d\n", mode);
            //printf("Now pointing at %c\n", (c != NULL) ? c -> c : '?');
        }
        l = l -> next;
        if(l != NULL){
            c = l -> head;
        }
        cnt++;
    }

    return getStatus(SUCCESS, 0, 0);
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
        if(ptr -> c > 47 && ptr -> c < 58){
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

