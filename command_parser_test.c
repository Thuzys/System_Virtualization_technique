#include "bach.h"
#include <string.h>

PBachCommand* command_parser(char *line, int *size);
void free_command(PBachCommand cmd);

int main(){
    int size;
    char *line = "grep abc > output.txt";
    PBachCommand* bach_commands = command_parser(line, &size);
    for (int i = 0; i < size; i++) {
        free_command(bach_commands[i]);
    }
    free(bach_commands);
    return 0;
}