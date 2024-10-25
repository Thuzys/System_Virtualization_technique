#include <stdio.h>
#include <unistd.h>

#define DATA_SIZE 16*1024*1024

char info[DATA_SIZE];
char data[DATA_SIZE] = {1};

int main() {
	printf("PID: %u\n", getpid());

	const long PAGE_SIZE = sysconf(_SC_PAGE_SIZE);
	printf("PAGE_SIZE: %ld\n", PAGE_SIZE);

	printf("#1 (press ENTER to continue)"); getchar();

	// a
	
	printf("#2 (press ENTER to continue)"); getchar();

	// b
	
	printf("#3 (press ENTER to continue)"); getchar();

	// c

	printf("#4 (press ENTER to continue)"); getchar();

	// d

	printf("#5 (press ENTER to continue)"); getchar();

	// e

	printf("END (press ENTER to continue)"); getchar();

	return 0;
}
