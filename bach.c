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


//add args to main
int main() {
    while (TRUE) {
        char line[MAX_LINE];
        printf("$: ");
        if (!fgets(line, MAX_LINE, stdin)) {
            break;
        }
        // parse the command
        // fun void xpto(BachCommand *cmd) {TODO()}
    }
}

void executecmd(PBachCommand cmd){	
    int child_pid;

    if ((child_pid = fork()) == -1) {
		perror("can't create new process");
		exit(1);
	}
	else if (child_pid == 0) {
		if (execvp(cmd.name, cmd.args) == -1) {
			perror2("error on exec %s", cmd.name);
			exit(1);
		}
	
	}
	else {
		waitpid(child_pid, NULL, 0);
	}

    return;
}
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