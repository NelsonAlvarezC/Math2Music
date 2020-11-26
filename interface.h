#pragma once

#include <stdio.h>
#include <stdlib.h>


void read_line_stdin(char* buffer, int size){
    char *p;
    fgets(buffer, size-1, stdin);
    if ((p=strchr(buffer, '\n')))
        *p = '\0';
}

int get_input_notes(char* buffer, int size){
    int done = 0;
    printf("Select input option\n 1. Standard input\n 2. File\n> ");
    read_line_stdin(buffer, size);
    if(buffer[0] == 0x31){
        printf("Write your notes\n> ");
        read_line_stdin(buffer, size);
    }
    else if(buffer[0] == 0x32){
        FILE *file;
        printf("Enter file name\n> ");
        read_line_stdin(buffer, size);
        if((file = fopen(buffer, "r"))){
            fscanf(file, "%[^\n]", buffer);
            strcat(buffer, "\0");
            printf("%s\n",buffer);
            fclose(file);
        }
        else
            fprintf(stderr,"[Read file] Couldn't open read file %s.\n",buffer);
    }
    else
        done = 1;
    return done;
}
