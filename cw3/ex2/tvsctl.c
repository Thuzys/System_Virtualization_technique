#include <sys/socket.h> // library for sockets
#include <sys/un.h> // library for unix domain sockets
#include <unistd.h> // standard symbolic constants and types
#include <stdio.h> // standard input/output
#include <stdlib.h> // standard library
#include <string.h> // string operations
#include "services.h" // services header

#define REQUEST_BUFFER_SIZE 256 // size of request buffer

/**
 * Send request to the server
 * 
 * @param request request string
 * 
 * @return void
 */
void send_request(const char *request) {
    int sock; // socket
    struct sockaddr_un srv_addr; // server address

    // Create socket
    if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) { // verify if socket is valid
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Set up the server address structure
    memset(&srv_addr, 0, sizeof(struct sockaddr_un));
    srv_addr.sun_family = AF_UNIX;
    strncpy(srv_addr.sun_path, SOCKET_PATH, sizeof(srv_addr.sun_path) - 1);

    // Connect to the server
    if (connect(sock, (struct sockaddr *) &srv_addr, sizeof(struct sockaddr_un)) == -1) {
        perror("connect");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Send the request
    if (write(sock, request, strlen(request)) == -1) {
        perror("write");
        close(sock);
        exit(EXIT_FAILURE);
    }

    close(sock);
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
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <command>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Close stdin
    close(STDIN_FILENO);

    // Construct the request string
    char request[REQUEST_BUFFER_SIZE] = {0};
    for (int i = 1; i < argc; i++) {
        strcat(request, argv[i]);
        if (i < argc - 1) {
            strcat(request, " ");
        }
    }

    // Send the request to the server
    send_request(request);

    exit(EXIT_SUCCESS);
}