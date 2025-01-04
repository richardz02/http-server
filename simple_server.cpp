#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 7890

void fatal(const char *message) {
    fprintf(stderr, "Fatal error: %s\n", message);
    exit(EXIT_FAILURE);
}

int main() {
    int sockfd, new_sockfd;
    struct sockaddr_in host_addr, client_addr;
    socklen_t sin_size;
    int recv_length = 1, yes = 1;
    char buffer[1024];

    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
        fatal("in socket");

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        fatal("setting socket option SO_REUSEADDR");

    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(PORT);
    host_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(host_addr.sin_zero), '\0', 8);

    if (bind(sockfd, (struct sockaddr*)&host_addr, sizeof(struct sockaddr)) == -1)
        fatal("binding to socket");

    if (listen(sockfd, 5) == -1)
        fatal("listening on socket");

    printf("Server is listening on port %d...\n", PORT);

    while (1) {
        sin_size = sizeof(struct sockaddr_in);
        new_sockfd = accept(sockfd, (struct sockaddr*)&client_addr, &sin_size);

        if (new_sockfd == -1)
            fatal("accepting connection");

        printf("Server: got connection from %s port %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        const char *response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n\r\n"
            "Hello world!\n";

        // Sends data to the client connected to the server via the new_sockfd
        send(new_sockfd, response, strlen(response), 0);

        recv_length = recv(new_sockfd, buffer, sizeof(buffer) - 1, 0);
        while (recv_length > 0) {
            buffer[recv_length] = '\0'; // Null-terminate the buffer
            printf("RECV (%d bytes): %s\n", recv_length, buffer);
            recv_length = recv(new_sockfd, buffer, sizeof(buffer) - 1, 0);
        }

        close(new_sockfd); // Close the connection
    }

    close(sockfd); // Close the listening socket
    return 0;
}
