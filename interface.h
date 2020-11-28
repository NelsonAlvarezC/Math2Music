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
    printf("Select input option (Another to quit)\n 1. Standard input\n 2. File\n> ");
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

char* get_output_name(){
    char* name = (char*)malloc(50);
    fprintf(stdout,"Write name of output file without extension\n> ");
    read_line_stdin(name, 46);
    strcat(name, ".wav\0");
    return name;
}

int ask_play_song(){
    int play = 1;
    char *buffer = (char*)malloc(10);
    fprintf(stdout, "Play song [Y/n]? ");
    read_line_stdin(buffer, 10);
    if(strlen(buffer) && ((buffer[0] == 'n') || (buffer[0] == 'N')))
            play = 0;
    free(buffer);
    return play;
}

char ask_filter_song(){
    char filter = 1;
    char *buffer = (char*)malloc(10);
    printf("Filter song (It may sound wrong with python player) [Y/n]? ");
    read_line_stdin(buffer, 10);
    if(strlen(buffer) && ((buffer[0] == 'n') || (buffer[0] == 'N')))
            filter = 0;
    if(filter){
        printf("Choose a note\n> ");
        read_line_stdin(buffer, 10);
        filter = buffer[0];
    }
    free(buffer);

    return filter;
}
