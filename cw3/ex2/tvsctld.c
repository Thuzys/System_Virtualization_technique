#include <stdlib.h> // standard library
#include <stdio.h> // standard input/output
#include <string.h> // string operations
#include <unistd.h> // standard symbolic constants and types
#include <errno.h> // system error numbers
#include <sys/socket.h> // main sockets header
#include <syslog.h> // system logging
#include <sys/un.h> // Unix domain sockets
#include <signal.h> // signal handling
#include <stdbool.h> // boolean types
#include <fcntl.h> // file control options
#include <pthread.h> // POSIX threads
#include "services.h" // services header

// constants
#define COMUNICATION_BUFFER_SIZE 256 // size of communication buffer
#define COMMAND_BUFFER_SIZE 256 // size of command buffer
#define PARAMS_BUFFER_SIZE 256 // size of parameters buffer
#define CMD_BUFFER_SIZE 512 // size of command buffer
#define SERVER_STDIN 3 // server standard input

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
 * Initialize signal handler
 * 
 * @return void
 */
void init() {
    signal(SIGTERM, sig_term_handler); // register signal handler
}

/**
 * Handle client
 * 
 * @param client_sock client socket
 * @return void
 */
void handle_client(int client_sock) {
    char buffer[COMUNICATION_BUFFER_SIZE];
    ssize_t num_read;

    // Read the request from the client
    num_read = read(client_sock, buffer, sizeof(buffer) - 1);
    if (num_read > 0) {
        buffer[num_read] = '\0'; // Null-terminate the string

        // Parse the command and parameters
        char command[COMMAND_BUFFER_SIZE];
        char params[PARAMS_BUFFER_SIZE] = "";
        sscanf(buffer, "%s %[^\n]", command, params);

        // Construct the command string to be executed
        char cmd[CMD_BUFFER_SIZE];
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
            syslog(LOG_ERR, "Unknown command: %s", buffer);
            close(client_sock);
            return;
        }

        // Execute the command
        system(cmd);
    }

    close(client_sock);
}

/**
 * Create bind socket
 * 
 * @param sock_name socket name
 * 
 * @return integer
 */
int create_bind_socket(const char* sock_name) {
    int sock; // socket
    struct sockaddr_un addr; // address
    sock = socket(AF_UNIX, SOCK_STREAM, 0); // create socket
    if (sock == -1) { // check if socket is valid
        perror("socket"); // print error
        return -1;
    }
    // remove old socket if exists
    if (unlink(sock_name) == -1 && errno != ENOENT) {
        perror("unlink"); // print error
        close(sock); // close socket
        return -2;
    }

    // bind socket
    memset(&addr, 0, sizeof(struct sockaddr_un)); // clear address
    addr.sun_family = AF_UNIX; // set address family
    strncpy(addr.sun_path, sock_name, sizeof(addr.sun_path) - 1); // copy socket name

    if (bind(sock, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) == -1) { // bind socket
        perror("bind"); // print error
        close(sock); // close socket
        return -3;
    }

    // set listen queue size
    if (listen(sock, BACKLOG) == -1) { // listen
        perror("listen"); // print error
        close(sock); // close socket
        return -4;
    }

    return sock; // return socket
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
    int sock, client_sock; // socket, client socket
    struct sockaddr_un client_addr; // client address
    socklen_t client_addr_len = sizeof(struct sockaddr_un); // client address length

    // Initialize signal handler
    init();

    // Open the system log
    openlog(SERVER_NAME, LOG_PID, LOG_DAEMON);

    sock = create_bind_socket(SOCKET_PATH); // create and bind socket
    if (sock == -1) { // check if socket is valid
        syslog(LOG_ERR, "Failed to create and bind socket");
        exit(EXIT_FAILURE); // exit with failure
    }

    while (true)
    {
        if (interrupted) { // check if interrupted
            break; // break loop
        }
        client_sock = accept(sock, (struct sockaddr*)&client_addr, &client_addr_len); // accept client
        if (client_sock == -1) { // check if client socket is valid 
            perror("accept"); // print error
            continue; // continue loop   
        }
        handle_client(client_sock); // handle client
    }
    close(sock); // close socket
    closelog(); // close system log
    exit(EXIT_SUCCESS); // exit with success
}