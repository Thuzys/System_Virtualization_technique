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

char* split(char *line, char *delim) {
    char *token = strtok(line, delim);
    return token;
}

//moments: 
// 1. read line from user
// 2. parse line into commands
//parsing 
 //     cat text.txt | grep abc > output.txt
 //     cat text.txt , grep abc > output.txt