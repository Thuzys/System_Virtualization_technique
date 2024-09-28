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
 */
typedef struct bach_command_t {
    int num_args;
    char *name;
    char **args;
} BachCommand, *PBachCommand;

// Functions signatures
PBachCommand* command_parser(char *line, int *size);
char** split(char* line, char* delim, int* size);
void trim(char* line);
void executecmd(PBachCommand cmd);
void free_command(PBachCommand bach_command);
void run_bach(char* line);

//test of split function
/*
*    int main(int argc, char *argv[]) {
*        int size;
*        char *line = "cat text.txt | grep abc > output.txt";
*       char** parts = split(line, PIPE_TOKEN, &size);
*       for (int i = 0; i < size; i++) {
*           printf("%s\n", parts[i]);
*       }
*       return 0;
*   }
*/

// test of command_parser function
/*
    int main(){
        int size;
        char *line = "cat text.txt | grep abc > output.txt";
        PBachCommand* bach_commands = command_parser(line, &size);
        for (int i = 0; i < size; i++) {
            PBachCommand bach_command = bach_commands[i];
            printf("Command: %s\n", bach_command->name);
            printf("Arguments: ");
            for (int j = 0; j < bach_command->num_args; j++) {
                printf("%s ", bach_command->args[j]);
            }
            printf("\n");
        }
        return 0;
    }
*/

/**
 * Run bach commands
 * The function reads a line from the standard input
 * The function parses the line into bach_commands
 * The function executes the bach_commands
 * The function returns nothing
 */
void run_bach(char* line) {
    int size;
    PBachCommand* bach_commands = command_parser(line, &size);
    for (int i = 0; i < size; i++) {
        PBachCommand bach_command = bach_commands[i];
        executecmd(bach_command);
    }
    for (int i = 0; i < size; i++) {
        free_command(bach_commands[i]);
    }
    free(bach_commands);
    return;
}

/**
 * Free the memory allocated for a bach_command
 * The memory allocated for the name and the arguments is freed
 * The memory allocated for the bach_command is freed
 * The function does not return anything
 */
void free_command(PBachCommand bach_command) {
    for (int i = 0; i < bach_command->num_args; i++) {
        free(bach_command->args[i]);
    }
    free(bach_command->args);
    free(bach_command);
    return;
}

/**
 * Execute a bach command
 * The command is executed in a child process
 * The parent process waits for the child to finish
 * The command is executed using execvp
 */
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

/**
 * Parse a line of commands into an array of bach_commands
 * Returns the number of generated bach_commands on size parameter
 * Returns a pointer to the array of bach_commands
 * The bach_commands array should be freed after use
 */
PBachCommand* command_parser(char *line, int *size) {
    int parts_size;
    char **parts = split(line, PIPE_TOKEN, &parts_size);
    
    // Allocate memory for the bach_commands array
    PBachCommand* bach_commands = (PBachCommand*)malloc((unsigned long)parts_size * sizeof(PBachCommand));
    if (bach_commands == NULL) {
        perror("Error allocating memory for bach_commands");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < parts_size; i++) {
        char* command = parts[i];
        int args_size;
        char** args = split(command, BLANK_CHAR, &args_size);
        // Allocate memory for the bach_command
        PBachCommand bach_command = malloc(sizeof(BachCommand));
        if (bach_command == NULL) {
            perror("Error allocating memory for bach_command");
            exit(EXIT_FAILURE);
        }
        // Allocate memory for args with an extra slot for the null terminator
        char** args_with_null = malloc((args_size + 1) * sizeof(char*));
        if (args_with_null == NULL) {
            perror("Error allocating memory for args_with_null");
            exit(EXIT_FAILURE);
        }

        // Copy arguments
        for (int j = 0; j < args_size; j++) {
            args_with_null[j] = args[j];
        }
        // Set the null terminator
        args_with_null[args_size] = NULL;

        bach_command->name = args[0];
        bach_command->args = args_with_null;
        bach_command->num_args = args_size;
        bach_commands[i] = bach_command;
        free(args);
    }
    *size = parts_size;
    return bach_commands;
}

/*
 * Split a string into parts using a delimiter
 * Returns the number of generated parts on size parameter
 * Retusns a pointer to the array of parts
 */
char** split(char* line, char* delim, int* size) {
    // Make a copy of the line to avoid modifying the original string
    char* dupLine = strdup(line);
    if (dupLine == NULL) {
        perror("Error duplicating the line");
        exit(EXIT_FAILURE);
    }

    char* token = strtok(dupLine, delim);
    int parts_size = 0;
    while (token != NULL) {
        parts_size++;
        token = strtok(NULL, delim);
    }
    
    // Allocate memory for the parts array
    char** parts = (char**)malloc((unsigned long)parts_size * sizeof(char*));
    if (parts == NULL) {
        perror("Error allocating memory for parts");
        exit(EXIT_FAILURE);
    }

    strcpy(dupLine, line);
    token = strtok(dupLine, delim);
    *size = parts_size;
    for (int i = 0; i < parts_size; i++) {
        char* part = strdup(token); // Allocate memory for each token
        if (part == NULL) {
            perror("Error allocating memory for token");
            // Free the allocated memory
            for (int j = 0; j < i; j++) {
                free(parts[j]);
            }
            free(parts);
            free(dupLine);
            exit(EXIT_FAILURE);
        }
        trim(part);
        parts[i] = part;
        token = strtok(NULL, delim);
    }
    free(dupLine);
    return parts;
}

/**
 * Trim a string by removing leading and trailing whitespaces
 */
void trim(char* line) {
    char* end;
    char* start = line;

    // Move the start pointer to the first non-whitespace character
    while (isspace((unsigned char)*start)) start++;

    // If the string is all spaces, set it to an empty string
    if (*start == 0) {
        *line = '\0';
        return;
    }

    // Move the end pointer to the last non-whitespace character
    end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end)) end--;

    // Set the new null terminator
    end[1] = '\0';

    // Move the trimmed string to the beginning of the original string
    memmove(line, start, end - start + 2);

    return;
}
