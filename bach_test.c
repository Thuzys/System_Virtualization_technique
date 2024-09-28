#include "bach.h"

int main(int argc, char *argv[]) {
    char *line = "echo hello world";
    run_bash(line);
    return 0;
}