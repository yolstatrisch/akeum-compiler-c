#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "program.h"
#include "struct.h"
#include "status.h"

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

    if(!isupper(ptr -> c)){
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

void play(PLAY *pl, NOTE *n){
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

STATUS getTimeSig(PROGRAM *p, LINE *l){
    CODE *ptr;
    int c = 0, val = 0, len = 0;

    ptr = l -> head;

    if(!isdigit(ptr -> c)){
        return getStatus(ERR_HEAD_TIME_NONE, l -> line, c);
    }

    while(isdigit(ptr -> c)){
        val *= 10;
        val += ptr -> c - 48;
        ptr = ptr -> next;
        c++;
    }

    if(ptr -> c != '-'){
        return getStatus(ERR_HEAD_TIME_FORMAT, l -> line, c);
    }

    ptr = ptr -> next;
    c++;

    if(!isdigit(ptr -> c)){
        return getStatus(ERR_HEAD_TIME_NONE, l -> line, c);
    }

    while(isdigit(ptr -> c)){
        len *= 10;
        len += ptr -> c - 48;
        ptr = ptr -> next;
        c++;
    }

    p -> value = val;
    p -> length = val;
    l -> head = ptr;

    return getStatus(SUCCESS, l -> line, 0);
}

void initRunStat(uint_fast8_t *i, int c){
    int flag = 0x1;

    while(c-- > 0){
        *i |= flag;
        flag <<= 1;
    }
}

void initPtrArr(CODE **c, LINE *l, int n){
    LINE *ptr = l;
    int cnt;

    for(cnt = 0; cnt < n; cnt++){
        *(c + cnt) = ptr -> head;
        ptr = ptr -> next;
    }
}

void initLastPl(NOTE **l, int n){
    int i;
    for(i = 0; i < n; i++){
        l[i] = (NOTE*)malloc(sizeof(NOTE));
        l[i] -> val = MIN;
        l[i] -> next = l[i] -> prev = NULL;
    }
}

void initLoop(LOOP **l, int n){
    int i;
    for(i = 0; i < n; i++){
        l[i] = NULL;
    }
}

voit initMode(int_fast8_t *mode, int n){
    int i;
    for(i = 0; i < n; i++){
        mode[i] = 0;
    }
}

STATUS playProgram(PROGRAM *p, PLAYLIST *pl){
    int count = p -> count;
    int value = p -> value;
    int length = p -> length;

    CODE **ptrArr = (CODE**)malloc(count * sizeof(CODE*));
    CODE *ptr;
    LINE *l = p -> line;

    NOTE *lastPlayed[count], *marker[26];
    LOOP *loop[count];

    uint_fast8_t runStatus = 0x0;
    uint8_t rptr = 0x0;

    int_fast8_t mode[count];

    initRunStat(&runStatus, count);
    initPtrArr(ptrArr, l, count);
    initLastPl(lastPlayed, count);
    initLoop(loop, count);
    initMode(mode, count);
    srand(time(NULL));

    pl = (PLAYLIST*)malloc(sizeof(PLAYLIST));
    pl -> count = 0;
    pl -> head = pl -> tail = NULL;

    while(runStatus != 0x0){
        if(*(ptrArr + rptr) != NULL){
            ptr = *(ptrArr + rptr);

            if(isupper(ptr -> c)){
                // Play Note
            }
            else if(islower(ptr -> c)){
                // Set Marker
            }
            else{
                switch(ptr -> c){
                    
                }
            }
        }
        else{
            runStatus &= ~(0x1 << rptr);
        }

        rptr = (rptr + 1) % count;
    }

    return getStatus(SUCCESS, 0, 0);
    //cutoff line

    while(l != NULL){
        initNoteToMin(last);

        loop = NULL;
        mode = 0;

        pl[cnt] = (PLAY*)malloc(sizeof(PLAY));
        pl[cnt] -> head = pl[cnt] -> tail = last;
        pl[cnt] -> cnt = 0;
        ptr = pl[cnt];

        while(c != NULL){
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

                temp -> val = tempval;
                temp -> next = NULL;
                last = temp;
                play(ptr, temp);
            }
            else if(islower(c -> c)){
                tempval = c -> c - 97;

                if(c -> next -> c != '=' || c -> next -> next -> c != c -> c){
                    marker[tempval] = last;
                }
                else{
                    add = last;
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
                            int e = 1;
                            while(e){
                                if(c -> c == ':' && c -> next -> c == '|' && c -> next -> next -> c == '|'){
                                    e = 0;
                                }
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

                        // TODO: check for size first (optimization)
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
                        c = c -> next;
                    }
                    break;
                case '?':
                    i = rand() % 12;

                    temp = (NOTE*)malloc(sizeof(NOTE));
                    temp -> val = i + value + mode + last -> val;
                    temp -> next = NULL;

                    last = temp;
                    play(ptr, temp);

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

    return getStatus(SUCCESS, 0, 0);
}
