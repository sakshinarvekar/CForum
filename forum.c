// forum.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "forum.h"

char current_user[50] = "Anonymous";

Topic *forum_head = NULL;

Topic *create_topic(const char *title) {
    Topic *new_topic = malloc(sizeof(Topic));
    if (!new_topic) return NULL;
    strncpy(new_topic->title, title, MAX_TITLE);
    new_topic->posts = NULL;
    new_topic->next = NULL;
    return new_topic;
}

Post *create_post(const char *author, const char *content) {
    Post *new_post = malloc(sizeof(Post));
    if (!new_post) return NULL;
    strncpy(new_post->author, author, MAX_AUTHOR);
    strncpy(new_post->content, content, MAX_CONTENT);
    new_post->next = NULL;
    return new_post;
}

void add_topic(Topic *new_topic) {
    if (!forum_head) {
        forum_head = new_topic;
    } else {
        Topic *temp = forum_head;
        while (temp->next)
            temp = temp->next;
        temp->next = new_topic;
    }
}

void add_post_to_topic(Topic *topic, Post *post) {
    if (!topic->posts) {
        topic->posts = post;
    } else {
        Post *temp = topic->posts;
        while (temp->next)
            temp = temp->next;
        temp->next = post;
    }
}

void print_forum() {
    if (!forum_head) {
        printf("\n📭 No topics have been created yet.\n");
        return;
    }

    printf("\n📚 Existing Topics:\n");
    Topic *t = forum_head;
    while (t) {
        printf("📌 Topic: %s\n", t->title);
        Post *p = t->posts;
        while (p) {
            printf("   👤 %s: %s\n", p->author, p->content);
            p = p->next;
        }
        t = t->next;
    }
}

void free_forum() {
    Topic *t = forum_head;
    while (t) {
        Post *p = t->posts;
        while (p) {
            Post *to_free = p;
            p = p->next;
            free(to_free);
        }
        Topic *to_free = t;
        t = t->next;
        free(to_free);
    }
    forum_head = NULL;
}

void save_forum(const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) return;

    Topic *t = forum_head;
    while (t) {
        fprintf(fp, "TOPIC|%s\n", t->title);
        Post *p = t->posts;
        while (p) {
            fprintf(fp, "POST|%s|%s\n", p->author, p->content);
            p = p->next;
        }
        t = t->next;
    }

    fclose(fp);
}

void load_forum(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return;

    char line[1024], *type, *part1, *part2;
    Topic *curr_topic = NULL;

    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = 0; // strip newline
        type = strtok(line, "|");
        part1 = strtok(NULL, "|");
        part2 = strtok(NULL, "|");

        if (strcmp(type, "TOPIC") == 0) {
            curr_topic = create_topic(part1);
            add_topic(curr_topic);
        } else if (strcmp(type, "POST") == 0 && curr_topic) {
            add_post_to_topic(curr_topic, create_post(part1, part2));
        }
    }

    fclose(fp);
}

