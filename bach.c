#include <stdio.h>
#include <stdlib.h>
#include <string.h>
<<<<<<< Updated upstream
#include <sys/wait.h>

#define MAX_LINE 1024
#define MAX_ARGS 64
=======
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LINE 1024
#define MAX_ARGS 20
>>>>>>> Stashed changes
#define TRUE 1

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