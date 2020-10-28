#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define STRING_MAX_SIZE 255

typedef struct List {
    char *string;
    struct List *next;
} List;

List *init_List() {
    List *list = (List *) malloc(sizeof(List));
    if (list) {
        list->string = NULL;
        list->next = NULL;
    }
    return list;
}

void pop(List *list) {
    List *current_list = list;
    while (current_list) {
        if (current_list->string) {
            free(current_list->string);
        }
        List *to_delete = current_list;
        current_list = current_list->next;
        free(to_delete);
    }
}

void print_strings(List *list) {
    List *current_list = list;
    while (current_list) {
        printf("%s\n", current_list->string);
        current_list = current_list->next;
    }
}

void push(List **list, char *string) {
    if (!*list) {
        *list = init_List();
        if (!*list) {
            perror("List didn't init\n");
            exit(-1);
        }
        (*list)->string = string;
    } else {
        List *last_list = *list;
        while (last_list->next) {
            last_list = last_list->next;
        }
        List *new_list = init_List();
        new_list->string = string;
        last_list->next = new_list;
        if (!new_list) {
            perror("List can't add more string\n");
            print_strings(*list);
            pop(*list);
            exit(-1);
        }
    }
}

int main() {
    char string[STRING_MAX_SIZE];
    List *list = NULL;
    printf("Enter your strings. String ends with \".\" :\n");
    if (fgets(string, STRING_MAX_SIZE, stdin) != NULL) {
        while (*(fgets(string, STRING_MAX_SIZE, stdin)) != '.') {
            size_t string_lenght = strlen(string);
            char *tmp = (char *) malloc(string_lenght);
            if (tmp) {
                memcpy(tmp, string, string_lenght);
                if (tmp[string_lenght - 1] == '\n') {
                    tmp[string_lenght - 1] = '\0';
                }
                push(&list, tmp);
            }
        }
        print_strings(list);
        pop(list);
    }
    else {
        perror("fgets return NULL\n");
        exit(-1);
    }
    return 0;
}
