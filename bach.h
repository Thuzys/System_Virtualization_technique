#ifndef BACH_H
#define BACH_H

#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE 1024
#define MAX_ARGS 10
#define MAX_ARG_LENGTH 100
#define TRUE 1

// Define the PBachCommand type and any other necessary types
typedef struct bach_command_t {
    int num_args;
    char name[MAX_ARG_LENGTH];
    char args[MAX_ARGS][MAX_ARG_LENGTH];
    FILE *input;
    FILE *output;
} BachCommand, *PBachCommand;

// Function prototypes
void execute_cmd(BachCommand cmd);
void command_parser(const char *line, PBachCommand bach_commands, int *size);
void run_bach(char* line);

#endif // BACH_H
