#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "forum.h"

#define GREEN   "\033[1;32m"
#define RED     "\033[1;31m"
#define YELLOW  "\033[1;33m"
#define CYAN    "\033[1;36m"
#define RESET   "\033[0m"

void show_menu() {
    int choice;
    char title[100], content[512];
    Topic *topic;
    int topic_index, i;

    while (1) {
        printf("\n╔══════════════════════════════════════╗\n");
        printf("║         🧩 " CYAN "Welcome to CForum" RESET "        ║\n");
        printf("║    A Terminal-Based Forum System    ║\n");
        printf("╚══════════════════════════════════════╝\n");
        printf("👤 Logged in as: %s\n", current_user);

        printf("\nMain Menu:\n");
        printf("────────────────────────────────────────\n");
        printf("1️⃣  View Topics\n");
        printf("2️⃣  Create a New Topic\n");
        printf("3️⃣  Post a Message to a Topic\n");
        printf("4️⃣  Exit and Save\n");
        printf("────────────────────────────────────────\n");
        printf("Select an option [1-4]: ");
        scanf("%d", &choice);
        getchar(); // consume newline

        switch (choice) {
            case 1:
                if (!forum_head) {
                    printf("\n📭 " YELLOW "No topics have been created yet.\n" RESET);
                    break;
                }

                printf("\n📚 " CYAN "Forum Topics:\n" RESET);
                printf("────────────────────────────────────────\n");
                topic = forum_head;
                i = 1;
                while (topic) {
                    printf("%d. 📌 %s\n", i++, topic->title);
                    Post *p = topic->posts;
                    if (!p) {
                        printf("   ┗ (No posts yet)\n");
                    }
                    while (p) {
                        printf("   ┗ 👤 %s: %s\n", p->author, p->content);
                        p = p->next;
                    }
                    topic = topic->next;
                }
                printf("────────────────────────────────────────\n");
                printf("(Press Enter to return to main menu)");
                getchar();
                break;

            case 2:
                printf("\n🆕 Create a New Topic\n");
                printf("────────────────────────────────────────\n");
                printf("Enter topic title: ");
                fgets(title, sizeof(title), stdin);
                title[strcspn(title, "\n")] = '\0';
                add_topic(create_topic(title));
                printf(GREEN "✅ Topic '%s' created successfully!\n" RESET, title);
                break;

            case 3:
                if (!forum_head) {
                    printf(RED "⚠️  No topics exist. Create one first!\n" RESET);
                    break;
                }

                printf("\n💬 Post a Message to a Topic\n");
                printf("────────────────────────────────────────\n");
                topic = forum_head;
                i = 1;
                while (topic) {
                    printf("  %d. %s\n", i++, topic->title);
                    topic = topic->next;
                }

                printf("Enter topic number: ");
                scanf("%d", &topic_index);
                getchar();

                topic = forum_head;
                for (i = 1; topic && i < topic_index; i++)
                    topic = topic->next;

                if (!topic) {
                    printf(RED "❌ Invalid topic number.\n" RESET);
                    break;
                }

                printf("💬 Posting as: %s\n", current_user);
                printf("Enter your message: ");
                fgets(content, sizeof(content), stdin);
                content[strcspn(content, "\n")] = '\0';

                add_post_to_topic(topic, create_post(current_user, content));
                printf(GREEN "✅ Post added successfully!\n" RESET);
                break;

            case 4:
                printf("\n👋 " YELLOW "Saving forum and exiting... Goodbye, %s!\n" RESET, current_user);
                return;

            default:
                printf(RED "❌ Invalid choice. Please enter 1-4.\n" RESET);
        }
    }
}
