#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#define DATA_SIZE 16*1024*1024
#define THREE_MB 3*1024*1024
#define CHUNK_SIZE 256

char info[DATA_SIZE];
char data[DATA_SIZE] = {1};

typedef void (*dummy_function_t)();

int main() {
	printf("PID: %u\n", getpid());

	const long PAGE_SIZE = sysconf(_SC_PAGE_SIZE);
	printf("PAGE_SIZE: %ld\n", PAGE_SIZE);

	printf("#1 (press ENTER to continue)"); getchar();

	// a
	for (int i = 0; i < THREE_MB; i += 1) {
		info[i] = 1;
	}
	
	
	printf("#2 (press ENTER to continue)"); getchar();

	// b
	int idx = 0;
	for (int i = 0; i < CHUNK_SIZE; i += 1) {
		char dummy = data[idx];
		idx += PAGE_SIZE;
	}

	
	printf("#3 (press ENTER to continue)"); getchar();

	// c
	if (fork() == 0) {
		printf("Child PID: %u\n", getpid());
		sleep(30);
		exit(0);
	}
	wait(NULL);

	printf("#4 (press ENTER to continue)"); getchar();

	// d
	void *lib = dlopen("./libDummy.so", RTLD_LAZY);

	printf("#5 (press ENTER to continue)"); getchar();

	// e
	dummy_function_t dummy_function = dlsym(lib, "dummy_function");
	dummy_function();

	printf("END (press ENTER to continue)"); getchar();

	return 0;
}
