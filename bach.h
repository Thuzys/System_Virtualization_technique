#ifndef BACH_H
#define BACH_H

#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE 1024
#define TRUE 1

// Define the PBachCommand type and any other necessary types
typedef struct {
    char* name;
    char** args;
    int num_args;
} BachCommand, *PBachCommand;

// Function prototypes
PBachCommand* command_parser(char *line, int *size);
void executecmd(PBachCommand cmd);
void free_command(PBachCommand cmd);
void run_bach(char* line);

#endif // BACH_H