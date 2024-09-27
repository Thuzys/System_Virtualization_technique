#include <ctype.h>
#include <string.h>
#include "utils.h"

// Function to trim whitespace from the beginning and end of a string
char* trim(char* str) {
    char* end;

    while(isspace((unsigned char)*str)) str++;

    if(*str == 0)  // All spaces?
        return str;

    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;

    end[1] = '\0';

    return str;
}