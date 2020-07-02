#include <stdio.h>
#include <stdlib.h>

int main(){
    union 
    {
        short s;
        char c[2];
        char cc;
    }un;

    un.s = 0x0102;
    if(un.c[0] == 2 && un.c[1] == 1){
        printf("little\n");
    }else if(un.c[0] == 1 && un.c[1] == 2){
        printf("big\n");
    }
    printf("%d\n", un.cc);
    return 0;
}