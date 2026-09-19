#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void handle_client(int client_socket)
{
    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    int bytes_read;

    memset(buffer, 0, sizeof(buffer));

    // Receive student data and ID from client
    bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);

    if (bytes_read <= 0)
    {
        close(client_socket);
        exit(0);
    }

    buffer[bytes_read] = '\0';

    // First line contains Student ID
    char *id = strtok(buffer, "\n");

    // Remaining data contains file records
    char *data = strtok(NULL, "");

    int found = 0;

    if (id != NULL && data != NULL)
    {
        char data_copy[BUFFER_SIZE];
        strcpy(data_copy, data);

        char *line = strtok(data_copy, "\n");

        while (line != NULL)
        {
            char record_id[20];

            sscanf(line, "%s", record_id);

            if (strcmp(record_id, id) == 0)
            {
                snprintf(response, sizeof(response),
                         "Record Found : %s", line);
                found = 1;
                break;
            }

            line = strtok(NULL, "\n");
        }
    }

    if (!found)
    {
        strcpy(response, "Record Not Found");
    }

    // Send response to client
    write(client_socket, response, strlen(response));

    close(client_socket);
    exit(0);
}

int main()
{
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket < 0)
    {
        perror("Socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket,
             (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        close(server_socket);
        exit(1);
    }

    if (listen(server_socket, 5) < 0)
    {
        perror("Listen failed");
        close(server_socket);
        exit(1);
    }

    printf("Server started...\n");
    printf("Waiting for clients...\n");

    while (1)
    {
        client_len = sizeof(client_addr);

        client_socket = accept(server_socket,
                               (struct sockaddr *)&client_addr,
                               &client_len);

        if (client_socket < 0)
        {
            perror("Accept failed");
            continue;
        }

        printf("Client connected.\n");

        // Create child process for each client
        pid_t pid = fork();

        if (pid < 0)
        {
            perror("Fork failed");
            close(client_socket);
        }
        else if (pid == 0)
        {
            // Child process
            close(server_socket);
            handle_client(client_socket);
        }
        else
        {
            // Parent process
            close(client_socket);
        }
    }

    close(server_socket);

    return 0;
}
