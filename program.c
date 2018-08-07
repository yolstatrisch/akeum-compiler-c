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

void play(PLAYLIST *pl, NOTE *n, int r, int t){
    static int playTime = -1, playCount = -1;

    PLAY *play = (PLAY*)malloc(sizeof(PLAY));
    play -> played = n;
    play -> clef = r;
    play -> next = NULL;

    if(playTime != t){
        playCount++;
        playTime = t;
    }
    play -> playTime = playCount;

    if(pl -> count == 0){
        play -> prev = NULL;
        pl -> head = pl -> tail = play;
    }
    else{
        play -> prev = pl -> tail;
        pl -> tail -> next = play;
        pl -> tail = play;
    }

    pl -> count++;
}

void playAll(PLAYLIST *pl){
    PLAY *p = pl -> head;
    while(p != NULL){
        printf("%d: %d\n", p -> playTime, p -> played -> val);
        p = p -> next;
    }
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

void initMode(int_fast8_t *mode, int n){
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

    NOTE *lastPlayed[count], *marker[26], *temp, *add;
    LOOP *loop[count];

    uint_fast8_t runStatus = 0x0;
    uint8_t lock = 0x0, rptr = 0x0, rnd;
    uint_fast64_t rtotal = 0x0, rcnt = 0x0;

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
        if(*(ptrArr + rptr) == NULL || !(runStatus & 0x1 << rptr)){
            runStatus &= ~(0x1 << rptr);
        }
        else{
            ptr = *(ptrArr + rptr);

            if(isupper(ptr -> c)){
                int note = (ptr -> c - 60) % 7;
                note = (note > 2) ? note * 2 - 10 : note * 2 - 9;

                temp = (NOTE*)malloc(sizeof(NOTE));

                if(ptr -> next -> c == '#'){
                    note++;
                    ptr = ptr -> next;
                }
                else if(ptr -> next -> c == 'b'){
                    note--;
                    ptr = ptr -> next;
                }

                note += value;
                note += mode[rptr];

                temp -> val = note;
                temp -> next = NULL;
                lastPlayed[rptr] = temp;
                play(pl, temp, rptr, rcnt);
            }
            else if(islower(ptr -> c)){
                int note = ptr -> c - 97;
                marker[note] = lastPlayed[rptr];
            }
            else{
                switch(ptr -> c){
                    case '|':
                        if(ptr -> next -> c == '|' && ptr -> next -> next -> c == ':'){
                            if(lastPlayed[rptr] -> val > 0 || lastPlayed[rptr] -> val == MIN){
                                LOOP *looptemp = (LOOP*)malloc(sizeof(LOOP));
                                looptemp -> count = lastPlayed[rptr] -> val - 1;
                                looptemp -> c = ptr -> next -> next;
                                looptemp -> next = NULL;

                                if(loop[rptr] == NULL){
                                    loop[rptr] = looptemp;
                                }
                                else{
                                    looptemp -> next = loop[rptr];
                                    loop[rptr] = looptemp;
                                }
                            }
                            else{
                                int e = 1;
                                while(e){
                                    if(ptr -> c == ':' && ptr -> next -> c == '|' && ptr -> next -> next -> c == '|'){
                                        e = 0;
                                    }
                                    ptr = ptr -> next;
                                }
                                ptr = ptr -> next;
                            }

                            ptr = ptr -> next -> next;
                        }
                        break;
                    case ':':
                        if(ptr -> next -> c == '|' && ptr -> next -> next -> c == '|'){
                            if(loop[rptr] -> count > 0){
                                loop[rptr] -> count--;
                                ptr = loop[rptr] -> c;
                            }
                            else if(loop[rptr] -> count == MIN){
                                ptr = loop[rptr] -> c;
                            }
                            else{
                                if(loop[rptr] -> next != NULL){
                                    LOOP *temp = loop[rptr];
                                    loop[rptr] = loop[rptr] -> next;
                                    free(temp);
                                }

                                ptr = ptr -> next -> next;
                            }
                        }
                        else{
                            return getStatus(ERR_INV_COMMAND, rptr, rcnt);
                        }
                        break;
                    case '+':
                        if(lastPlayed[rptr] -> val != MIN){
                            mode[rptr] += lastPlayed[rptr] -> val;
                        }
                        else{
                            return getStatus(ERR_PLAY_NULL, rptr, rcnt);
                        }
                        break;
                    case '-':
                        if(lastPlayed[rptr] -> val != MIN){
                            mode[rptr] -= lastPlayed[rptr] -> val;
                        }
                        else{
                            return getStatus(ERR_PLAY_NULL, rptr, rcnt);
                        }
                        break;
                    case '.':
                        mode[rptr] = 0;
                        break;
                    case '%':
                        temp = (NOTE*)malloc(sizeof(NOTE));
                        temp -> val = MIN;
                        temp -> next = NULL;

                        play(pl, temp, rptr, rcnt);
                        lastPlayed[rptr] = temp;
                        break;
                    case '~':
                        if(lastPlayed[rptr] -> val == 0){
                            int e = 1;
                            while(e || ptr -> next == NULL){
                                if(ptr -> c == ':' && ptr -> next -> c == '|' && ptr -> next -> next -> c == '|'){
                                    e = 0;
                                }
                                ptr = ptr -> next;
                            }
                            ptr = ptr -> next;
                        }
                        break;
                    case '=':
                        if(islower(ptr -> next -> c)){
                            int tempval = ptr -> next -> c - 97;

                            temp = (NOTE*)malloc(sizeof(temp));
                            temp -> val = marker[tempval] -> val + value + mode[rptr];
                            temp -> next = NULL;

                            play(pl, temp, rptr, rcnt);
                            lastPlayed[rptr] = temp;
                            ptr = ptr -> next;
                        }
                        else if(isdigit(ptr -> next -> c) || ptr -> next -> c == '-'){
                            int tempval = pl -> count;
                            int rel = 0, sign = 1, i;

                            if(ptr -> next -> c == '-'){
                                sign *= -1;
                            }
                            else{
                                rel = ptr -> next -> c - '0';
                            }

                            CODE *tempptr = ptr -> next -> next;
                            while(isdigit(tempptr -> c)){
                                rel = rel * 10 + ((tempptr -> c - '0') * sign);
                                ptr = tempptr;
                                tempptr = tempptr -> next;
                            }

                            if(abs(rel) > tempval){
                                return getStatus(ERR_PLAY_OOB, 0, 0);
                            }

                            // TODO: check for size first (optimization)
                            PLAY *plptr;
                            if(rel >= 0){
                                plptr = pl -> head;
                                while(rel > 1){
                                    plptr = plptr -> next;
                                    rel--;
                                }
                            }
                            else{
                                plptr = pl -> tail;
                                rel++;
                                while(rel < -1){
                                    plptr = plptr -> prev;
                                    rel++;
                                }
                            }

                            plptr -> played -> val += value;
                            plptr -> played -> val += mode[rptr];

                            temp = (NOTE*)malloc(sizeof(NOTE));
                            temp -> val = plptr -> played -> val;
                            temp -> next = NULL;

                            play(pl, temp, rptr, rcnt);
                            lastPlayed[rptr] = temp;
                            ptr = ptr -> next;
                        }
                        break;
                    case '?':
                        rnd = rand() % 12;

                        temp = (NOTE*)malloc(sizeof(NOTE));
                        temp -> val = rnd + value + mode[rptr] + lastPlayed[rptr] -> val;
                        temp -> next = NULL;

                        lastPlayed[rptr] = temp;
                        play(pl, temp, rptr, rcnt);
                        break;
                }
            }
        }

        *(ptrArr + rptr) = ptr -> next;

        rtotal++;
        rptr = rtotal % count;
        rcnt = floor(rtotal / count);
    }

    playAll(pl);
    return getStatus(SUCCESS, 0, 0);
}
