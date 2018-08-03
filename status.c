#include <stdio.h>
#include <stdlib.h>
#include "status.h"

STATUS getStatus(int s, int l, int c){
    STATUS st;

    st.status = s;
    st.line = l;
    st.c = c;

    return st;
}

void printStatus(STATUS stat){
    if(stat.status != SUCCESS){
        printf("ERROR: %d at clef %d : %d\n", stat.status, stat.line, stat.c);
        exit(stat.status);
    }
}
