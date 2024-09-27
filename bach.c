#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LINE 1024
#define MAX_ARGS 20
#define TRUE 1

#define EMPTY_CHAR '/0'       
#define OUTPUT_TOKEN '>'     
#define INPUT_TOKEN '<' 
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
} BachCommand, *PBachCommand;

void command_parser(char *line, PBachCommand bach_commands[MAX_ARGS]);
int split(char *line, char *delim, char *parts[], int parts_size);
char* trim(char* line);
void executecmd(PBachCommand cmd);

//add args to main
int main(int argc, char *argv[]) {
    PBachCommand bach_commands[MAX_ARGS];
    while (TRUE) {
        char line[MAX_LINE];
        printf("$: ");
        if (!fgets(line, MAX_LINE, stdin)) {
            break;
        }
        command_parser(line, bach_commands);
        for (size_t i = 0; i < sizeof(bach_commands); i++)
        {
            if (bach_commands[i] == NULL) {
                break;
            }
            executecmd(bach_commands[i]);
        }
    }
}

void executecmd(PBachCommand cmd){	
    int child_pid;

    if ((child_pid = fork()) == -1) {
		perror("can't create new process");
		exit(1);
	}
	else if (child_pid == 0) {
		if (execvp(cmd->name, cmd->args) == -1) {
			perror("exec error");
			exit(1);
		}
	}
	else {
		waitpid(child_pid, NULL, 0);
	}
    return;
}


/* int main() {
    // Test command line input
    char testLine[] = "cat text.txt | grep   abc > output.txt | |";

    PBachCommand bach_commands[MAX_ARGS];

    // Call the command parser with the test line
    command_parser(testLine, bach_commands);

    return 0;
} */

void command_parser(char *line, PBachCommand bach_commands[MAX_ARGS]) {
    char *parts[MAX_ARGS];
    int parts_size = split(line, PIPE_TOKEN, parts, MAX_ARGS);
    for (int i = 0; i < parts_size; i++) {
        char *args[MAX_ARGS];
        char *command = parts[i];
        int args_size = split(command, BLANK_CHAR, args, MAX_ARGS);

        if (args_size == 0) { continue; }

        // Create a new BachCommand
        PBachCommand bach_command = malloc(sizeof(BachCommand));
        bach_command->name = args[0];

        for (int j = 0; j < args_size; j++) {
            char *arg = args[j];
                bach_command->args[j] = args[j];
                break;
        }
        bach_command->args[args_size] = NULL;
	
		bach_commands[i] = bach_command;
	}
}


//returns the number or element of the array parts
int split(char *line, char *delim, char *parts[], int parts_size) {
    char *trimmed_line = trim(line);
    char *token = strtok(trimmed_line, delim);
    int i = 0;
    while (token != NULL) {
        parts[i] = token;
        token = strtok(NULL, delim);
        i++;
    }
    return i;
}

char* trim(char* line) {
    char* end;

    while (isspace((unsigned char)*line)) line++;

    if (*line == 0) 
        return line;

    end = line + strlen(line) - 1;
    while (end > line && isspace((unsigned char)*end)) end--;

    end[1] = '\0';

    return line;
}
