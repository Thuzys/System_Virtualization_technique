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

char *EMPTY_CHAR  = "/0";       
char *OUTPUT_TOKEN = ">";     
char *INPUT_TOKEN = "<";
char *PIPE_TOKEN = "|";
char *BLANK_CHAR = " ";

/**
 * Struct to hold a simple command information.
 * The name of the command,
 * The arguments,
 */
typedef struct bach_command_t {
    int num_args;
    char *name;
    char **args;
    FILE *input;
    FILE *output;
} BachCommand, *PBachCommand;
// The size of the bach_command struct is 40 bytes

// Functions signatures
PBachCommand* command_parser(char *line, int *size);
char** split(char* line, char* delim, int* size);
void trim(char* line);
void executecmd(PBachCommand cmd);
void free_command(PBachCommand bach_command);
void run_bach(char* line);

// int main(int argc, char *argv[]) {
//     (void)argc; // Mark argc as unused
//     (void)argv; // Mark argv as unused
//     while (TRUE)
//     {
//         char line[MAX_LINE];
//         printf("$: ");
//         if(!fgets(line, MAX_LINE, stdin)) {
//             break;
//         }
//         if (strcmp(line, "exit\n") == 0) {
//             break;
//         }
//         run_bach(line);
//     }
//     return 0;
// }

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
    if (bach_command->input != NULL) {
        fclose(bach_command->input);
    }
    if (bach_command->output != NULL) {
        fclose(bach_command->output);
    }
    free(bach_command);
    return;
}

/**
 * Execute a bach command
 * The command is executed in a child process
 * The parent process waits for the child to finish
 * The command is executed using execvp
 */
void executecmd(PBachCommand cmd) {
    int child_pid;
    int saved_stdin = -1;
    int saved_stdout = -1;

    // Save the original file descriptors
    if (cmd->input != NULL) {
        saved_stdin = dup(STDIN_FILENO);
        if (saved_stdin == -1) {
            perror("dup");
            exit(1);
        }
        dup2(fileno(cmd->input), STDIN_FILENO);
    }

    if (cmd->output != NULL) {
        saved_stdout = dup(STDOUT_FILENO);
        if (saved_stdout == -1) {
            perror("dup");
            exit(1);
        }
        dup2(fileno(cmd->output), STDOUT_FILENO);
    }

    if ((child_pid = fork()) == -1) {
        perror("can't create new process");
        exit(1);
    } else if (child_pid == 0) {
        if (execvp(cmd->name, cmd->args) == -1) {
            perror("exec error");
            exit(1);
        }
    } else {
        waitpid(child_pid, NULL, 0);
    }

    // Restore the original file descriptors
    if (cmd->input != NULL) {
        dup2(saved_stdin, STDIN_FILENO);
        close(saved_stdin);
    }

    if (cmd->output != NULL) {
        dup2(saved_stdout, STDOUT_FILENO);
        close(saved_stdout);
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
    char **parts = split(line, PIPE_TOKEN, &parts_size); // a malloc is done to allocate memory for the parts array // 3  -> 2
    
    // Allocate memory for the bach_commands array
    PBachCommand* bach_commands = (PBachCommand*)malloc((unsigned long)parts_size * sizeof(PBachCommand)); // a malloc is done to allocate memory for the bach_commands array -> 3
    if (bach_commands == NULL) {
        perror("Error allocating memory for bach_commands");
        for (int i = 0; i < parts_size; i++) {
            free(parts[i]);
        }
        free(parts);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < parts_size; i++) {
        
        char* command = parts[i]; // command is a pointer to the parts array, with is malloced
        int args_size;
        char** args = split(command, BLANK_CHAR, &args_size); // malloc args; -> 8
        free(command); // free the memory allocated for the command on the parts array -> 7
        
        // Allocate memory for the bach_command
        PBachCommand bach_command = malloc(sizeof(BachCommand)); // -> 8
        if (bach_command == NULL) {
            perror("Error allocating memory for bach_command");
            for (int j = 0; j < args_size; j++) {
                free(args[j]);
            }
            free(args);
            for (int j = 0; j < parts_size; j++) {
                free(parts[j]);
            }
            free(parts);
            free(bach_commands);
            exit(EXIT_FAILURE);
        }
        
        int actual_args_size = 0;
        // Allocate memory for args with an extra slot for the null terminator
        char** args_with_null = malloc(((unsigned long)args_size + 1) * (unsigned long)sizeof(char*)); 
        if (args_with_null == NULL) {
            perror("Error allocating memory for args_with_null");
            for (int j = 0; j < args_size; j++) {
                free(args[j]);
            }
            free(args);
            for (int j = 0; j < parts_size; j++) {
                free(parts[j]);
            }
            free(parts);
            free(bach_commands);
            exit(EXIT_FAILURE);
        }

        int j = 0;
        while (j < args_size) {
            char *arg = args[j];
            if (strcmp(arg, INPUT_TOKEN) == 0) {
                FILE *input = fopen(args[j + 1], "r");
                if (input == NULL) {
                    perror("Error opening input file");
                    for (int j = 0; j < args_size; j++) {
                        free(args[j]);
                    }
                    free(args);
                    for (int j = 0; j < parts_size; j++) {
                        free(parts[j]);
                    }
                    free(parts);
                    free(bach_commands);
                    for (int j = 0; j < args_size+1; j++) {
                        free(args_with_null[j]);
                    }
                    free(args_with_null);
                    exit(EXIT_FAILURE);
                }
                bach_command->input = input;
                j += 2;
                free(arg);
                free(args[j+1]);
            }
            else if (strcmp(arg, OUTPUT_TOKEN) == 0) {
                FILE *output = fopen(args[j + 1], "w");
                if (output == NULL) {
                    perror("Error opening output file");
                    for (int j = 0; j < args_size; j++) {
                        free(args[j]);
                    }
                    free(args);
                    for (int j = 0; j < parts_size; j++) {
                        free(parts[j]);
                    }
                    free(parts);
                    free(bach_commands);
                    for (int j = 0; j < args_size+1; j++) {
                        free(args_with_null[j]);
                    }
                    free(args_with_null);
                    exit(EXIT_FAILURE);
                }
                bach_command->output = output;
                j += 2;
                free(arg);
                free(args[j+1]);
            }
            else {
                args_with_null[actual_args_size] = arg;
                actual_args_size++;
                j++;
            }
        }

        bach_command->name = args[0];
        bach_command->args = args_with_null;
        bach_command->num_args = args_size;
        bach_commands[i] = bach_command;
        free(args);
    }
    *size = parts_size;
    free(parts);
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

