#include <stdio.h> // standard input/output
#include <string.h> // string operations
#include <unistd.h> // standard symbolic constants and types
#include <stdlib.h> // standard library
#include <errno.h> // system error numbers
#include <sys/socket.h> // main sockets header
#include <syslog.h> // system logging
#include <sys/un.h> // Unix domain sockets
#include <signal.h> // signal handling
#include <stdbool.h> // boolean types
#include <fcntl.h> // file control options
#include <pthread.h> // POSIX threads
#include <systemd/sd-daemon.h> // socket activation
#include "services.h" // services header

// constants
#define COMUNICATION_BUFFER_SIZE 256 // size of communication buffer
#define COMMAND_BUFFER_SIZE 256 // size of command buffer
#define PARAMS_BUFFER_SIZE 256 // size of parameters buffer
#define CMD_BUFFER_SIZE 512 // size of command buffer
#define SERVER_STDIN 3 // server standard input
#define SERVER_NAME "tvsctld" // server name

// size of pending connections queue
#define BACKLOG 128

// global
volatile bool interrupted = false;
int srv_sock;

// signal handler

/**
 * Signal handler for SIGTERM
 * 
 * @param signum signal number
 * @return void
 */
void sig_term_handler(int signum) {
    interrupted = true; // set global flag
    close(srv_sock); // close server socket
}

/**
 * Initialize log
 * 
 * @return void
 */
void init_log() {
    openlog(SERVER_NAME, LOG_PID, LOG_DAEMON); // open system log
}

/**
 * Initialize signal handler
 * 
 * @return void
 */
void init() {
    init_log(); // initialize log
    signal(SIGTERM, sig_term_handler); // register signal handler
}

/**
 * Process connection
 * 
 * @param cfd client socket
 * @return void
 */
void process_connection(int cfd) {
    char buffer[COMUNICATION_BUFFER_SIZE];
    ssize_t num_read;

    // Read the request from the client
    while ((num_read = read(cfd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[num_read] = '\0'; // Null-terminate the string
        char cmd[CMD_BUFFER_SIZE]; // command buffer
        // Parse the command and parameters
        char command[COMMAND_BUFFER_SIZE];
        char params[PARAMS_BUFFER_SIZE] = "";
        sscanf(buffer, "%s %[^\n]", command, params); // parse command and parameters
        // Use strcmp to compare strings
        if (strcmp(command, "reset") == 0) {
            snprintf(cmd, sizeof(cmd), "/opt/isel/tvs/scripts/reset.sh %s", params);
        } else if (strcmp(command, "inc") == 0) {
            snprintf(cmd, sizeof(cmd), "/opt/isel/tvs/scripts/inc.sh %s", params);
        } else if (strcmp(command, "stop") == 0) {
            snprintf(cmd, sizeof(cmd), "/opt/isel/tvs/scripts/stop.sh %s", params);
        } else if (strcmp(command, "start") == 0) {
            snprintf(cmd, sizeof(cmd), "/opt/isel/tvs/scripts/start.sh %s", params);
        } else if (strcmp(command, "dec") == 0) {
            snprintf(cmd, sizeof(cmd), "/opt/isel/tvs/scripts/dec.sh %s", params);
        } else if (strcmp(command, "status") == 0) {
            snprintf(cmd, sizeof(cmd), "/opt/isel/tvs/scripts/status.sh %s", params);
        } else {
            syslog(LOG_ERR, "Unknown command: %s", command);
            continue;
        }
        switch (command) {
        case "reset":
            snprintf(cmd, sizeof(cmd), "/opt/isel/tvs/scripts/reset.sh %s", params);
            break;
        case "inc":
            snprintf(cmd, sizeof(cmd), "/opt/isel/tvs/scripts/inc.sh %s", params);
            break;
        case "stop":
            snprintf(cmd, sizeof(cmd), "/opt/isel/tvs/scripts/stop.sh %s", params);
            break;
        case "start":
            snprintf(cmd, sizeof(cmd), "/opt/isel/tvs/scripts/start.sh %s", params);
            break;
        case "dec":
            snprintf(cmd, sizeof(cmd), "/opt/isel/tvs/scripts/dec.sh %s", params);
            break;
        case "status":
            snprintf(cmd, sizeof(cmd), "/opt/isel/tvs/scripts/status.sh %s", params);
            break;
        default:
            break;
        }
        int child_pid = fork(); // fork process
        if (child_pid == -1) { // check if child process is valid
            perror("fork"); // print error
            close(cfd); // close client socket
            return; // return
        }
        if (child_pid == 0) { // check if child process
            // Redirect the standard input to the client socket
            if (dup2(cfd, STDOUT_FILENO) == -1) { // duplicate file descriptor
                perror("dup2"); // print error
                close(cfd); // close client socket
                exit(EXIT_FAILURE); // exit with failure
            }
            // Execute the command
            system(cmd); // execute command
        }
    }
    wait(NULL); // wait for child process
    close(cfd); // close client socket
}

/**
 * Dispatch connection
 * 
 * @param arg argument
 * @return void
 */
void* dispatch_connection(void* arg) {
    int cfd = *(int*)arg; // client socket
    process_connection(cfd); // process connection
    return NULL; // return null
}

/**
 * Run server
 * 
 * @return void
 */
void run() {
    int client_sock; // client socket
    syslog(LOG_INFO, "Server started"); // log server start
    while (!interrupted) { // check if interrupted
        struct sockaddr_un client_addr; // client address
        socklen_t client_addr_len = sizeof(struct sockaddr_un); // client address length
        client_sock = accept(srv_sock, (struct sockaddr*)&client_addr, &client_addr_len); // accept client
        if (interrupted) { // check if interrupted
            break; // break loop
        }
        if (client_sock == -1) { // check if client socket is valid
            perror("accept"); // print error
            continue; // continue loop
        }
        pthread_t thread; // thread
        pthread_create(&thread, NULL, dispatch_connection, &client_sock); // create thread
        pthread_detach(thread); // detach thread
    }
    syslog(LOG_INFO, "Server stopped"); // log server stop
}

/**
 * Main function
 * 
 * @param argc argument count
 * @param argv argument vector
 * 
 * @return integer
 */
int main(int argc, char *argv[]) {
    // Initialize the server and signal handler
    init();

    // sd_listen_fds(0) is used to detect socket activation

    int nfd = sd_listen_fds(0); // detect socket activzation
    if (nfd != 1) { // check if number of file descriptors is not 1
        syslog(LOG_ERR, "Invalid number of file descriptors: %d", nfd); // log error
        exit(EXIT_FAILURE); // exit with failure
    }

    srv_sock = SD_LISTEN_FDS_START; // get server socket

    run(); // run server

    close(srv_sock); // close socket
    closelog(); // close system log
}