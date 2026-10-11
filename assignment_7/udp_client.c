#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 30027
#define BUFFER_SIZE 2048

int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in server_addr;
    socklen_t server_len;
    char input[BUFFER_SIZE];
    char message[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    const char *roll;
    ssize_t received;

    if (argc != 3) {
        fprintf(stderr,
                "Usage: %s <server_ip> <roll>\n", argv[0]);
        return 1;
    }

    roll = argv[2];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0) {
        perror("socket");
        return 1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, argv[1],
                  &server_addr.sin_addr) != 1) {
        fprintf(stderr, "Invalid server IP address\n");
        close(sockfd);
        return 1;
    }

    server_len = sizeof(server_addr);

    printf("Connected to UDP server %s:%d\n",
           argv[1], PORT);
    printf("Type SUM|n|numbers, MAX|n|numbers, "
           "MIN|n|numbers, AVG|n|numbers, or EXIT.\n");

    while (1) {
        printf("> ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            /* EOF: send the exit command and terminate. */
            snprintf(message, sizeof(message),
                     "%s#EXIT", roll);

            if (sendto(sockfd, message, strlen(message), 0,
                       (struct sockaddr *)&server_addr,
                       server_len) < 0) {
                perror("sendto");
            }

            break;
        }

        /* Remove the newline from the input. */
        input[strcspn(input, "\r\n")] = '\0';

        /* Handle a line that did not fit in the input buffer. */
        if (strchr(input, '\0') == input + sizeof(input) - 1) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {
                /* Discard the remainder of the line. */
            }
            fprintf(stderr, "Input is too long. Try again.\n");
            continue;
        }

        /* EXIT must be sent with the roll-number prefix. */
        if (strcmp(input, "EXIT") == 0) {
            snprintf(message, sizeof(message),
                     "%s#EXIT", roll);

            if (sendto(sockfd, message, strlen(message), 0,
                       (struct sockaddr *)&server_addr,
                       server_len) < 0) {
                perror("sendto");
            }

            break;
        }

        /* Automatically prepend the roll-number prefix. */
        int length = snprintf(message, sizeof(message),
                              "%s#%s", roll, input);

        if (length < 0 || (size_t)length >= sizeof(message)) {
            fprintf(stderr, "Request is too long.\n");
            continue;
        }

        if (sendto(sockfd, message, strlen(message), 0,
                   (struct sockaddr *)&server_addr,
                   server_len) < 0) {
            perror("sendto");
            continue;
        }

        received = recvfrom(
            sockfd, response, sizeof(response) - 1, 0,
            NULL, NULL
        );

        if (received < 0) {
            perror("recvfrom");
            continue;
        }

        response[received] = '\0';
        printf("Server response: %s\n", response);
    }

    close(sockfd);
    printf("Client terminated.\n");

    return 0;
}
