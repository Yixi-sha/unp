#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>

#include "../wrap_func.h"

int main(int argc, char* argv[]){
    
    struct hostent *retHostent = NULL;
    char *ptr = NULL, **pptr = NULL;
    char str[INET_ADDRSTRLEN];

    while(--argc > 0){
        ptr = *++argv;
        if((retHostent = gethostbyname(ptr)) == NULL){
            err_msg("gethostbyname ");
            continue;
        }
        printf("official name is: %s\n", retHostent->h_name);

        for(pptr = retHostent->h_aliases; *pptr != NULL; ++pptr)
            printf("alias: %s\n", *pptr);
        
        if(retHostent->h_addrtype == AF_INET){
            for(pptr = retHostent->h_addr_list; *pptr != NULL; ++pptr)
                printf("address: %s\n", inet_ntop(retHostent->h_addrtype, *pptr, str, sizeof(str)));
        }else{
            err("unkonw address type");
        }
    }

    exit(0);
}
