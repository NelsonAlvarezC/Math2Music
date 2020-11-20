#include <stdio.h>
#include <stdlib.h>
#include <string.h>


extern void math2music(char*, int);

int main(){
    char* buffer = (char*)malloc(100);
    strcpy(buffer, "CDEFfgDE\0");
    
    math2music(buffer, strlen(buffer));

    free(buffer);

    return 0;
}
