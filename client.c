// client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 9000
#define BUFFER_SIZE 2048

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    printf("✅ Connected to server.\n");

    char name[50], buffer[BUFFER_SIZE], input[BUFFER_SIZE];
    int bytes;

    // Get prompt from server and send username
    bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0) {
        printf("❌ Server disconnected.\n");
        return 1;
    }
    buffer[bytes] = '\0';
    printf("%s", buffer);

    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';  // remove newline
    send(sock, name, strlen(name), 0); // send raw name only

    while (1) {
        bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) {
            printf("❌ Server disconnected.\n");
            break;
        }

        buffer[bytes] = '\0';
        printf("%s", buffer);

        fgets(input, sizeof(input), stdin);
        send(sock, input, strlen(input), 0);

        if (strncmp(input, "4", 1) == 0)
            break;
    }

    close(sock);
    return 0;
}