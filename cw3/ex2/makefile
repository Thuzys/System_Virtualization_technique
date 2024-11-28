CC = gcc

CFLAGS = -g -Wall -Iinclude

TVS_SRV_SRC = tvsctld.c
TVS_SRV_CLI = tvsctl.c

all: tvsctld tvsctl

tvsctld: $(TVS_SRV_SRC)
	$(CC) -o tvsctld $(TVS_SRV_SRC) $(CFLAGS) -lsystemd

tvsctl: $(TVS_SRV_CLI)
	$(CC) -o tvsctl $(TVS_SRV_CLI) $(CFLAGS)

install:
	./install.sh

uninstall:
	./uninstall.sh

clean:
	rm -rf *.o tvsctld tvsctl
