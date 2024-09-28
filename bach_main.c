#include "bach.h"
#include <string.h>

void run_bach(char* line);

int main(int argc, char *argv[]) {
    (void)argc; // Mark argc as unused
    (void)argv; // Mark argv as unused
    while (TRUE)
    {
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