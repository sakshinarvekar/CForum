#include <stdio.h>
#include <string.h>
#include "forum.h"

void show_menu(); // from ui.c

int main() {
    printf("👤 Enter your name to log in: ");
    fgets(current_user, sizeof(current_user), stdin);
    current_user[strcspn(current_user, "\n")] = '\0'; // remove newline

    load_forum("forum.txt");
    show_menu();
    save_forum("forum.txt");
    free_forum();
    return 0;
}
