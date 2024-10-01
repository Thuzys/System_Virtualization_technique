#include "bach.h"
#include <string.h>

char** split(char* line, char* delim, int* size);


// NOTE: No memory leak in this function
int main() {
    int size;
    char *line = "cat text.txt | grep abc > output.txt";
    char** parts = split(line, PIPE_TOKEN, &size);
    for (int i = 0; i < size; i++) {
        printf("%s\n", parts[i]);
    }
    for (int i = 0; i < size; i++) {
        free(parts[i]);
    }
    free(parts);
    return 0;
}