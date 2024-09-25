#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"

#define MAX_LINE = 1024
#define MAX_ARGS 20
#define TRUE 1

#define OUTPUT_TOKEN ">"
#define INPUT_TOKEN "<"
#define PIPE_TOKEN "|"
#define BLANK_CHAR " "

/**
 * Struct to hold a simple command information.
 * The name of the command,
 * The arguments,
 * The redirections if they exist.
 */
typedef struct bach_command_t {
    char *name;
    char *args[MAX_ARGS];
    char *redir_in;
    char *redir_out;
} BachCommand, *PBachCommand;


void command_parser(char *line);
char* trim(char *str);

int main() {
    printf(trim("  hello world  "));
    return 0;
}

//add args to main
// int main() {
//     while (TRUE) {
//         char line[MAX_LINE];
//         printf("$: ");
//         if (fgets(line, MAX_LINE, stdin) == NULL) {
//             break;
//         }
//     }
//     return 0;
// }

void command_parser(char *line) {
    while (line != NULL) {
        char *command = split(line, PIPE_TOKEN);
        char *trimmed_command = trim(command);
        char *args[MAX_ARGS];
        int i = 0;
        while (trimmed_command != NULL) {
            args[i] = split(trimmed_command, BLANK_CHAR);
            i++;
        }
    }
}

//returns the number or element of the array parts
int split(char *line, char *delim, char *parts[], int parts_size) {
    char *token = strtok(line, delim);
    while (/* condition */)
    {
        /* code */
    }
    
    return token;
}

//moments: 
// 1. read line from user
// 2. parse line into commands
//parsing 
 //     cat text.txt | grep abc > output.txt
 //     cat text.txt , grep abc > output.txt