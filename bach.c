#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int MAX_LINE = 1024;
define TRUE 1;

//add args to main
int main() {
    while (TRUE) {
        char line[MAX_LINE];
        printf("$: ");
        if (!fgets(line, MAX_LINE, stdin)) {
            break;
        }
    }
    
}