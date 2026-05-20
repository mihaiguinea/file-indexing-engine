#include <stdio.h>
#include <stdlib.h>
#include <string.h>

///
////
/////   STRUCTURES
////
///

typedef struct FileRef {
    struct File *file;
    struct FileRef *next;
} FileRef;
// Singly linked list of references to files

typedef struct Tree {
    char ch;               // The character inside each node
    struct Tree *child;    // First child node
    struct Tree *sibling;  // Next sibling node
    FileRef *files;        // List of references to files, which exists ONLY in terminal nodes
    int is_terminal;       // Flag indicating 1 if the node is terminal and 0 otherwise
} Tree;

typedef struct KeywordNode {
    char word[101];
    struct KeywordNode *next;
} KeywordNode;
// Singly linked list of keywords

typedef struct File {
    char id[101];               // Name/ID of the file
    int score;                  // Relevance score
    KeywordNode *keywords;      // List of keywords associated with the file
    struct File *prev;
    struct File *next;
} File;

typedef struct {
    File *head;  // First file in the doubly linked list of files
    File *tail;  // Last file in the doubly linked list of files
} FileList;

///
////
/////   FILE LIST OPERATIONS
////
///

FileList *createList() {
    FileList *list = malloc(sizeof(FileList));
    list->head = NULL;
    list->tail = NULL;
    return list;
}
// Function that creates an empty file list

File *findFile(FileList *list, const char *id) {
    File *iter = list->head;

    while (iter != NULL) {
        if (strcmp(iter->id, id) == 0){
            return iter;
        }
        iter = iter->next;
    }

    return NULL;
}
// Function that traverses the file list from head to tail to search for a file with the given ID and returns a pointer to it

File *createFile(const char *id, int score) {
    File *f = malloc(sizeof(File));

    strcpy(f->id, id);
    f->score = score;
    f->keywords = NULL;
    f->prev = f->next = NULL;

    return f;
}
// 

void addFileToList(FileList *list, File *file) {
    file->prev = list->tail;
    file->next = NULL;

    if(list->tail != NULL){
        list->tail->next = file;
    }
    else{
        list->head = file;
    }
    list->tail = file;
}
// Function that appends a file to the end of the file list
// If the list is empty, the file becomes both head and tail

void removeFileFromList(FileList *list, File *file) {
    if(file->prev != NULL){
        file->prev->next = file->next;
    }
    else{
        list->head = file->next;
    }
    if(file->next != NULL){
        file->next->prev = file->prev;
    }
    else{
        list->tail = file->prev;
    }
}
// Function that extracts a file node from the file list


int hasKeyword(File *file, const char *word) {
    KeywordNode *iter = file->keywords;

    while(iter != NULL){
        if(strcmp(iter->word, word) == 0){
            return 1;
        }
        iter = iter->next;
    }

    return 0;
}
// Function that returns 1 if a keyword exists in the file's keyword list, or 0 otherwise

void addKeywordToFile(File *file, const char *word) {
    if(hasKeyword(file, word) == 1){
        return;
    }
    KeywordNode *k = malloc(sizeof(KeywordNode));

    strcpy(k->word, word);
    k->next = file->keywords;
    file->keywords = k;
}
// Inserts a given keyword at the beginning of the file's keyword list, if it is not already present

void removeKeywordFromFile(File *file, const char *word) {
    KeywordNode *iter = file->keywords;
    KeywordNode *prev = NULL;

    while(iter != NULL){
        if (strcmp(iter->word, word) == 0){
            if(prev !=
