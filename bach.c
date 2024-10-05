#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/wait.h>

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
 * 
 * @param line The string to be trimmed
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

/**
 * Splits a given string into tokens based on a specified delimiter.
 *
 * @param str The input string to be split.
 * @param delim The delimiter string used to split the input string.
 * @param result An array to store the resulting tokens. Each token is stored as a string.
 * @param size A pointer to an integer where the number of tokens found will be stored.
 */
void split(const char *str, const char *delim, char result[][MAX_ARG_LENGTH], int *size) {
    char *token; 
    char temp[MAX_LINE_LENGTH]; // Temporary array to store the input string
    strncpy(temp, str, MAX_LINE_LENGTH); // Make a copy of the input string 
    token = strtok(temp, delim);
    *size = 0;
    while (token != NULL && *size < MAX_ARGS) {
        trim(token); // Trim leading and trailing whitespaces
        strncpy(result[*size], token, MAX_ARG_LENGTH); 
        token = strtok(NULL, delim); // Get the next token
        (*size)++; // Increment the number of tokens found
    }
}


/**
 * Parses a command line string into a list of BachCommand structures.
 *
 * @param line The command line string to be parsed.
 * @param bach_commands An array of BachCommand structures to store the parsed commands.
 * @param size A pointer to an integer where the number of commands found will be stored.
 */
void command_parser(const char *line, PBachCommand bach_commands, int *size) {
    // Split the input line into individual commands separated by '|'
    char parts[MAX_COMMANDS][MAX_ARG_LENGTH];
    int parts_size;
    split(line, "|", parts, &parts_size);

    // Set the number of commands found
    *size = parts_size;

    // Iterate over each command part
    for (int i = 0; i < parts_size; i++) {
        // Split each command part into arguments separated by spaces
        char args[MAX_ARGS][MAX_ARG_LENGTH];
        int args_size;
        split(parts[i], " ", args, &args_size);

        // Copy the command name (first argument) to the bach_commands array
        strncpy(bach_commands[i].name, args[0], MAX_ARG_LENGTH);
        int actual_args_size = 0;
        bach_commands[i].input = 0;  // Initialize input file descriptor to 0 (standard input)
        bach_commands[i].output = 0; // Initialize output file descriptor to 0 (standard output)
        int j = 0;

        // Iterate over the arguments to handle input/output redirection and store arguments
        while (j < args_size) {
            if (args[j][0] == '<') {
                // Handle input redirection
                bach_commands[i].input = open(args[j + 1], O_RDONLY);
                if (bach_commands[i].input == -1) {
                    perror("open");
                    exit(1);
                }
                j += 2; // Skip the next argument as it is the input file
            } else if (args[j][0] == '>') {
                // Handle output redirection
                bach_commands[i].output = open(args[j + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (bach_commands[i].output == -1) {
                    perror("open");
                    exit(1);
                }
                j += 2; // Skip the next argument as it is the output file
            } else {
                // Store the argument in the bach_commands array
                strncpy(bach_commands[i].args[actual_args_size], args[j], MAX_ARG_LENGTH);
                actual_args_size++;
                j++;
            }
        }
        // Null-terminate the arguments array
        bach_commands[i].args[actual_args_size][0] = '\0';
        bach_commands[i].num_args = actual_args_size; // Set the number of arguments
    }
}

/**
 * Executes a pipeline of commands not limited by 2 commands.
 *
 * @param cmd_list An array of BachCommand structures representing the commands in the pipeline.
 * @param cmd_count The number of commands in the pipeline.
 */
void execute_pipeline_v2(PBachCommand cmd_list, int cmd_count) {
    int pipe_fd[2];
    int input_fd = 0; // Initially, the input is from the standard input
    int child_pid;

    for (int i = 0; i < cmd_count; i++) {
        if (pipe(pipe_fd) == -1) { // create a pipe
                perror("pipe");
                exit(1);
            }
        if ((child_pid = fork()) == -1) { // fork a child process
            perror("fork");
            exit(1);
        }
        if (child_pid == 0) {
            if (i == 0) { // if this is the first command
                if (cmd_list[i].input != 0) {  // if the command has an input file
                    dup2(cmd_list[i].input, STDIN_FILENO); // set the input to the input file
                    close(cmd_list[i].input);
                }
            } else {
                dup2(input_fd, STDIN_FILENO); // set the input to the previous command's output
                close(input_fd);
            }
            if (i < cmd_count - 1) { // if this is not the last command
                dup2(pipe_fd[1], STDOUT_FILENO); // set the output to the write end of the pipe
            } else { //if this is the last command
                if (cmd_list[i].output != 0) { // if the command has an output file
                    dup2(cmd_list[i].output, STDOUT_FILENO); //set the output to the standard output
                    close(cmd_list[i].output);
                }
            }
            close(pipe_fd[0]); // close the read end of the pipe
            char *args[MAX_ARGS + 1];  // create an array of arguments
            for (int j = 0; j < cmd_list[i].num_args; j++) {
                args[j] = cmd_list[i].args[j];
            }
            args[cmd_list[i].num_args] = NULL;
            execvp(cmd_list[i].name, args); // execute the command
            perror("execvp");
            exit(1);
        } else {
            waitpid(child_pid, NULL, 0); // wait for the child process to finish
            close(pipe_fd[1]);
            input_fd = pipe_fd[0];
        }
    }
}

/**
 * Executes a pipeline of commands limited to 2 commands.
 *
 * @param cmd_list An array of BachCommand structures representing the commands in the pipeline.
 * @param cmd_count The number of commands in the pipeline.
 */
void execute_pipeline(PBachCommand cmd_list, int cmd_count) {
    int pipe_fd[2];
    int input_fd = 0; // Initially, the input is from the standard input
    int child_pid;

    child_pid = fork(); // fork a child process
    if (child_pid == -1) { // check for fork error
        perror("fork");
        exit(1);
    } 
    if (child_pid == 0) { // if this is the child process
        if (pipe(pipe_fd) == -1) {
            perror("pipe");
            exit(1);
        }
        int second_child_pid = fork(); // fork another child process
        if (second_child_pid == -1) { // check for fork error
            perror("fork");
            exit(1);
        }
        if (second_child_pid == 0) {
            dup2(pipe_fd[0], STDIN_FILENO); // set the input to the previous command's output
            close(pipe_fd[0]); // close the previous command's output
            close(pipe_fd[1]); // close the write end of the pipe
            if (cmd_list[cmd_count - 1].output != 0) {
                dup2(cmd_list[cmd_count - 1].output, STDOUT_FILENO);
                close(cmd_list[cmd_count - 1].output);
            }
            char *args[MAX_ARGS + 1];
            for (int j = 0; j < cmd_list[1].num_args; j++) {
                args[j] = cmd_list[1].args[j];
            }
            args[cmd_list[1].num_args] = NULL;
            execvp(cmd_list[1].name, args); // execute the command
            perror("execvp");
            exit(1);
        } else {
            dup2(pipe_fd[1], STDOUT_FILENO); // set the output to the next command's input
            close(pipe_fd[0]); // close the read end of the pipe
            close(pipe_fd[1]); // close the write end of the pipe
            if (cmd_list[0].input != 0) {
                dup2(cmd_list[0].input, STDIN_FILENO);
                close(cmd_list[0].input);
            }
            char *args[MAX_ARGS + 1]; 
            for (int j = 0; j < cmd_list[0].num_args; j++) {
                args[j] = cmd_list[0].args[j];
            }
            args[cmd_list[0].num_args] = NULL;
            execvp(cmd_list[0].name, args); // execute the command
            perror("execvp");
            exit(1);
        }
    } else {
        waitpid(child_pid, NULL, 0); // wait for the child process to finish
    }
}

/**
 * Executes a simple command that is not part of a pipeline.
 *
 * @param cmd The BachCommand structure representing the command to be executed.
 */
void execute_simple_cmd(BachCommand cmd) {
    int child_pid;

    if (strcmp(cmd.name, "cd") == 0) { // check if the command is 'cd'
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
        if (cmd.input != 0) { // if the command has an input file
            dup2(cmd.input, STDIN_FILENO);
            close(cmd.input);
        }
        if (cmd.output != 0) { // if the command has an output file
            dup2(cmd.output, STDOUT_FILENO);
            close(cmd.output);
        }
        char *args[MAX_ARGS + 1];
        for (int i = 0; i < cmd.num_args; i++) {
            args[i] = cmd.args[i];
        }
        args[cmd.num_args] = NULL;
        execvp(cmd.name, args); // execute the command
        perror("execvp");
        exit(1);
    } else if (child_pid == -1) { 
        perror("fork");
    } else {
        waitpid(child_pid, NULL, 0); // wait for the child process to finish
    }
}

/**
 * Executes a command line string in the Bach shell.
 *
 * @param line The command line string to be executed.
 */
void run_bach(char* line) {
    BachCommand bach_commands[MAX_COMMANDS]; // array to store the commands
    int size; // number of commands found

    command_parser(line, bach_commands, &size); // parse the command line

    if (size == 1) { // if there is only one command
        execute_simple_cmd(bach_commands[0]);
        return;
    }
    else { // if there are multiple commands
        execute_pipeline_v2(bach_commands, size);
    }
}
