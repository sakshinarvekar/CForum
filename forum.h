// forum.h
#ifndef FORUM_H
#define FORUM_H

#define MAX_AUTHOR 50
#define MAX_CONTENT 512
#define MAX_TITLE 100

typedef struct Post {
    char author[MAX_AUTHOR];
    char content[MAX_CONTENT];
    struct Post *next;
} Post;

typedef struct Topic {
    char title[MAX_TITLE];
    Post *posts;
    struct Topic *next;
} Topic;

// Global head of topic list
extern Topic *forum_head;

// Basic operations
Topic *create_topic(const char *title);
Post *create_post(const char *author, const char *content);
void add_post_to_topic(Topic *topic, Post *post);
void add_topic(Topic *new_topic);

// Utility
void print_forum();
void free_forum();
void save_forum(const char *filename);
void load_forum(const char *filename);

extern char current_user[50];

void handle_view_topics(int sock);
void handle_create_topic(int sock, char *title);
void handle_prepare_post_to_topic(int sock, char *input, char *temp_topic);
void handle_submit_post_to_topic(int sock, char *message, char *temp_topic, char *username);


#endif
