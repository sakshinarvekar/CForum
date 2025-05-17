// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>
#include "forum.h"

#define PORT 9000
#define BUFFER_SIZE 2048

#define GREEN   "\033[1;32m"
#define RED     "\033[1;31m"
#define YELLOW  "\033[1;33m"
#define CYAN    "\033[1;36m"
#define RESET   "\033[0m"

void send_menu(int sock, const char *username) {
    char menu[BUFFER_SIZE];
    snprintf(menu, sizeof(menu),
        "\n\033[1;36m╔══════════════════════════════════════╗\033[0m\n"
        "║         🧩 %sWelcome to CForum%s         ║\n"
        "║    A Terminal-Based Forum System    ║\n"
        "╚══════════════════════════════════════╝\n"
        "👤 Logged in as: %s\n"
        "\nMain Menu:\n"
        "────────────────────────────────────────\n"
        "1️⃣  View Topics\n"
        "2️⃣  Create a New Topic\n"
        "3️⃣  Post a Message to a Topic\n"
        "4️⃣  Exit and Save\n"
        "────────────────────────────────────────\n"
        "Select an option [1-4]: ", CYAN, RESET, username);
    send(sock, menu, strlen(menu), 0);
}

void *handle_client(void *arg) {
    int sock = *(int *)arg;
    free(arg);

    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    int bytes_read;
    int state = 0;
    char temp_topic[100];
    char username[50] = "Anonymous";

    send(sock, "Enter your name: ", 19, 0);
    bytes_read = recv(sock, username, sizeof(username) - 1, 0);
    if (bytes_read <= 0) return NULL;
    username[bytes_read] = '\0';
    username[strcspn(username, "\n")] = '\0';

    send_menu(sock, username);

    while ((bytes_read = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_read] = '\0';
        buffer[strcspn(buffer, "\n")] = '\0';
        response[0] = '\0';

        if (state == 0) {
            int choice = atoi(buffer);
            if (choice == 1) {
                Topic *t = forum_head;
                if (!t) strcpy(response, "📭 \033[1;33mNo topics available.\033[0m\n");
                else {
                    strcat(response, "📚 \033[1;36mForum Topics:\033[0m\n");
                    int idx = 1;
                    while (t) {
                        char line[512];
                        snprintf(line, sizeof(line), "%d. 📌 %s\n", idx++, t->title);
                        strcat(response, line);
                        Post *p = t->posts;
                        if (!p) strcat(response, "   ┗ (No posts yet)\n");
                        while (p) {
                            snprintf(line, sizeof(line), "   ┗ 👤 %.50s: %.400s\n", p->author, p->content);
                            line[sizeof(line) - 1] = '\0';
                            strcat(response, line);
                            p = p->next;
                        }
                        t = t->next;
                    }
                }
                strcat(response, "(Press Enter to return to main menu)\n");
                send(sock, response, strlen(response), 0);
                recv(sock, buffer, sizeof(buffer), 0); // wait for Enter
            } else if (choice == 2) {
                send(sock, "Enter topic title: ", 21, 0);
                state = 1;
                continue;
            } else if (choice == 3) {
                Topic *t = forum_head;
                if (!t) {
                    strcpy(response, "⚠️  \033[1;31mNo topics exist. Create one first!\033[0m\n");
                    send(sock, response, strlen(response), 0);
                    send_menu(sock, username);
                    continue;
                }
                strcat(response, "🧵 Available Topics:\n");
                int idx = 1;
                while (t) {
                    char line[256];
                    snprintf(line, sizeof(line), "%d. %s\n", idx++, t->title);
                    strcat(response, line);
                    t = t->next;
                }
                strcat(response, "Enter topic number to reply to: ");
                send(sock, response, strlen(response), 0);
                state = 2;
                continue;
            } else if (choice == 4) {
                snprintf(response, sizeof(response), "\n👋 \033[1;33mSaving forum and exiting... Goodbye, %s!\033[0m\n", username);
                send(sock, response, strlen(response), 0);
                break;
            } else {
                strcpy(response, "❌ \033[1;31mInvalid option.\033[0m\n");
                send(sock, response, strlen(response), 0);
            }
        } else if (state == 1) {
            add_topic(create_topic(buffer));
            save_forum("forum.txt");
            snprintf(response, sizeof(response), "\033[1;32m✅ Topic '%.100s' created successfully!\033[0m\n", buffer);
            state = 0;
            send(sock, response, strlen(response), 0);
        } else if (state == 2) {
            int selected = atoi(buffer);
Topic *t = forum_head;
int i = 1;
while (t && i < selected) {
    t = t->next;
    i++;
}

            if (!t) {
                snprintf(response, sizeof(response), "❌ \033[1;31mInvalid topic number.\033[0m\n");
                state = 0;
                send(sock, response, strlen(response), 0);
            } else {
                strcpy(temp_topic, t->title);
                send(sock, "Enter your message: ", 22, 0);
                state = 3;
                continue;
            }
        } else if (state == 3) {
            Topic *t = forum_head;
            while (t && strcmp(t->title, temp_topic) != 0) t = t->next;
            if (!t) {
                snprintf(response, sizeof(response), "❌ Topic '%s' not found.\n", temp_topic);
            } else {
                add_post_to_topic(t, create_post(username, buffer));
                save_forum("forum.txt");
                strcpy(response, "\033[1;32m✅ Message posted successfully!\033[0m\n");
            }
            state = 0;
            send(sock, response, strlen(response), 0);
        }

        send_menu(sock, username);
    }

    close(sock);
    return NULL;
}

int main() {
    load_forum("forum.txt");

    int server_fd, client_fd;
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));
    listen(server_fd, 5);
    printf("🖥️  Server listening on port %d...\n", PORT);

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr *)&addr, &addrlen);
        printf("🔗 New client connected!\n");
        int *client_sock = malloc(sizeof(int));
        *client_sock = client_fd;
        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, client_sock);
        pthread_detach(tid);
    }

    return 0;
}