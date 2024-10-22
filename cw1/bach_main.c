#include "bach.h"
#include <string.h>
#include <signal.h>

void run_bach(char* line);

void sigint_handler(int sig) { 
    printf("\n");
}

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    while (TRUE)
    {
		signal(SIGINT, sigint_handler);
        char line[MAX_LINE];
        printf("$: ");
        if(!fgets(line, MAX_LINE, stdin)) {
            break;
        }
        if (strcmp(line, "exit\n") == 0) {
            break;
        }
        run_bach(line);
    }
    return 0;
}
