#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int MAX_LINE = 1024;
#define TRUE 1

/**
 * Struct to hold a simple command information.
 * The name of the command,
 * The arguments,
 * The redirections if they exist.
 */
typedef struct bach_command_t {
    char *name;
    char *args;
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
    }
    
}
