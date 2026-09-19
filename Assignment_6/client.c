#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main()
{
    int sock;
    struct sockaddr_in server_addr;

    char data[BUFFER_SIZE] = "";
    char line[200];
    char student_id[20];
    char response[BUFFER_SIZE];

    FILE *file;

    // Open data.txt
    file = fopen("data.txt", "r");

    if (file == NULL)
    {
        perror("Unable to open data.txt");
        return 1;
    }

    // Read complete file
    while (fgets(line, sizeof(line), file) != NULL)
    {
        strcat(data, line);
    }

    fclose(file);

    // Ask user for Student ID
    printf("Enter Student ID to search : ");
    scanf("%s", student_id);

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0)
    {
        perror("Socket creation failed");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to server
    if (connect(sock,
                (struct sockaddr *)&server_addr,
                sizeof(server_addr)) < 0)
    {
        perror("Connection failed");
        close(sock);
        return 1;
    }

    // Prepare message:
    // Student ID + file data
    char message[BUFFER_SIZE];

    snprintf(message, sizeof(message),
             "%s\n%s", student_id, data);

    // Send message to server
    write(sock, message, strlen(message));

    // Receive server response
    memset(response, 0, sizeof(response));

    read(sock, response, sizeof(response) - 1);

    printf("Server Response : %s\n", response);

    close(sock);

    return 0;
}
