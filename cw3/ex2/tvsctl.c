#include <sys/socket.h> // library for sockets
#include <sys/un.h> // library for unix domain sockets
#include <unistd.h> // standard symbolic constants and types
#include <stdio.h> // standard input/output
#include <stdlib.h> // standard library
#include <string.h> // string operations
#include "services.h" // services header

#define REQUEST_BUFFER_SIZE 256 // size of request buffer
#define NITERS 10 // number of iterations

int create_client_socket() {
    return socket(AF_UNIX, SOCK_STREAM, 0); // create socket
}

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
    close(0); // close standard input (non-interactive)

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <command>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int cli_sock = create_client_socket(); // create client socket
    if (cli_sock < 0) { // check if client socket is valid
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un srv_addr; // server address
    memset(&srv_addr, 0, sizeof(struct sockaddr_un)); // clear server address
    srv_addr.sun_family = AF_UNIX; // set address family
    strncpy(srv_addr.sun_path, SOCKET_NAME, sizeof(srv_addr.sun_path) - 1); // copy socket path

    if (connect(cli_sock, (struct sockaddr *) &srv_addr, sizeof(struct sockaddr_un)) == -1) { // connect to server
        perror("connect"); // print error
        close(cli_sock); // close socket
        exit(EXIT_FAILURE);
    }

    int pid = getpid(); // get process id

    // Construct the request string
    char request[REQUEST_BUFFER_SIZE] = {0};
    for (int i = 1; i < argc; i++) {
        strcat(request, argv[i]);
        if (i < argc - 1) {
            strcat(request, " ");
        }
    }
        
	write(cli_sock, request, strlen(request)); // write request to socket
	printf("Sent request: %s\n", request); // print request
	read(cli_sock, request, REQUEST_BUFFER_SIZE); // read response from socket
	printf("Received response: %s\n", request); // print response
	read(cli_sock, request, REQUEST_BUFFER_SIZE); // read response from socket
	printf("Received response: %s\n", request); // print response
	read(cli_sock, request, REQUEST_BUFFER_SIZE); // read response from socket
	printf("Received response: %s\n", request); // print response
    printf("Client %d done\n", pid); // print client done
    close(cli_sock); // close socket
    
    exit(EXIT_SUCCESS);
}
