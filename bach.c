#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

#define MAX_COMMANDS 10
#define MAX_ARGS 10
#define MAX_ARG_LENGTH 100
#define MAX_LINE_LENGTH 1000

typedef struct bach_command_t {
    int num_args;
    int input;
    int output;
    char name[MAX_ARG_LENGTH];
    char args[MAX_ARGS][MAX_ARG_LENGTH];
} BachCommand, *PBachCommand;

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


void split(const char *str, const char *delim, char result[][MAX_ARG_LENGTH], int *size) {
    char *token;
    char temp[MAX_LINE_LENGTH];
    strncpy(temp, str, MAX_LINE_LENGTH);
    token = strtok(temp, delim);
    *size = 0;
    while (token != NULL && *size < MAX_ARGS) {
        trim(token);
        strncpy(result[*size], token, MAX_ARG_LENGTH);
        token = strtok(NULL, delim);
        (*size)++;
    }
}



void command_parser(const char *line, PBachCommand bach_commands, int *size) {
    char parts[MAX_COMMANDS][MAX_ARG_LENGTH];
    int parts_size;
    split(line, "|", parts, &parts_size);

    *size = parts_size;

    for (int i = 0; i < parts_size; i++) {
        char args[MAX_ARGS][MAX_ARG_LENGTH];
        int args_size;
        split(parts[i], " ", args, &args_size);

        strncpy(bach_commands[i].name, args[0], MAX_ARG_LENGTH);
        int actual_args_size = 0;
        bach_commands[i].input = 0;
        bach_commands[i].output = 0;
        int j = 0;
        while (j < args_size) {
            if (args[j][0] == '<') {
                bach_commands[i].input = open(args[j + 1], O_RDONLY);
                if (bach_commands[i].input == -1) {
                    perror("open");
                    exit(1);
                }
                j += 2;
            } else if (args[j][0] == '>') {
                bach_commands[i].output = open(args[j + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (bach_commands[i].output == -1) {
                    perror("open");
                    exit(1);
                }
                j += 2;
            } else {
                strncpy(bach_commands[i].args[j], args[j], MAX_ARG_LENGTH);
                actual_args_size++;
                j++;
            }
        }
        bach_commands[i].args[args_size][0] = '\0';
        bach_commands[i].num_args = actual_args_size;
    }
}

void execute_cmd(BachCommand cmd) {
    int child_pid;

    if (strcmp(cmd.name, "cd") == 0) {
        if (cmd.num_args < 1) {
            fprintf(stderr, "cd: missing argument\n");
        } else {
            if (chdir(cmd.args[1]) != 0) {
                perror("cd");
            }
        }
        return;
    }

    if ((child_pid = fork()) == 0) {
        if (cmd.input != 0) {
            dup2(cmd.input, STDIN_FILENO);
        }
        if (cmd.output != 0) {
            dup2(cmd.output, STDOUT_FILENO);
        }
        char *args[MAX_ARGS + 1];
        for (int i = 0; i < cmd.num_args; i++) {
            args[i] = cmd.args[i];
        }
        args[cmd.num_args + 1] = NULL;
        execvp(cmd.name, args);
    } else if (child_pid == -1) {
        perror("fork");
    } else {
        waitpid(child_pid, NULL, 0);
    }
    
}

void run_bach(char* line) {
    BachCommand bach_commands[MAX_COMMANDS];
    int size;

    command_parser(line, bach_commands, &size);

    for (int i = 0; i < size; i++) {
        execute_cmd(bach_commands[i]);
    }
}


// int main() {
//     char line[MAX_LINE_LENGTH] = "ls\n";
//     BachCommand bach_commands[MAX_COMMANDS];
//     int size;

//     command_parser(line, bach_commands, &size);

//     for (int i = 0; i < size; i++) {
//         execute_cmd(bach_commands[i]);
//     }

//     return 0;
// }