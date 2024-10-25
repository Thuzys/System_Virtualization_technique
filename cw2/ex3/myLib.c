// mylib.c
#include <stdio.h>

#define ONE_HUNDRED_AND_TWENTY_EIGHT_KB 128 * 1024

// Dummy data to occupy ~512KB
char dummy_data[512 * 1024] = {1};

// Dummy code to occupy ~4KB
void dummy_function() {
    for (int i = 0; i < ONE_HUNDRED_AND_TWENTY_EIGHT_KB; ++i) {
        dummy_data[i] = 1;
    }
}
