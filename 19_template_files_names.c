#include <stdio.h>
#include <string.h>
#include <dirent.h>

#define MAX_SIZE 200

void simple_text(char *const text) {
    int count = 1;
    size_t length = strlen(text);
    for (int i = 0; i < length; i++)
        if ('*' != text[i - 1] || '*' != text[i])
            text[count++] = text[i];
    text[count] = 0;
}

int exist(const int i, const int j) {
    return (i >= 0 && j >= 0);
}

int conformity(const char *word, const char *text) {
    int array[MAX_SIZE][MAX_SIZE];
    size_t word_len = strlen(word);
    size_t text_len = strlen(text);
    for (int i = 0; i < word_len; i++)
        for (int j = 0; j < text_len; j++)
            array[i][j] = 0;
    array[0][0] = (text[0] == '*' || text[0] == '?' || text[0] == word[0]);
    for (int i = 0; i < word_len; i++) {
        for (int j = 0; j < text_len; j++) {
            switch (text[j]) {
                case '?':
                    array[i][j] = (exist(i - 1, j - 1) && array[i - 1][j - 1]) || (0 == i && 1 == j && '*' == text[j - 1]);
                    break;
                case '*':
                    array[i][j] = (exist(i - 1, j) && array[i - 1][j]) || (exist(i, j - 1) && array[i][j - 1]);
                    break;
                default:
                    array[i][j] = (word[i] == text[j])  && ((exist(i - 1, j - 1) && array[i - 1][j - 1]) || (0 == i && 1 == j && '*' == text[j - 1]));
                    break;
            }
        }
    }
    return array[word_len - 1][text_len -1];
}


int main() {
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char text[MAX_SIZE] = {0};
    char old_text[MAX_SIZE] = {0};
    int count = 0, bool = 0;
    printf("Mask: ");
    fgets(text, MAX_SIZE, stdin);
    if (text[strlen(text) - 1] == '\n')
        text[strlen(text) - 1] = 0;
    if (strchr(text, '/') != NULL) {
        fprintf(stderr, "Incorrect, U have '/' \n");
        return -1;
    }
    dir = opendir(".");
    if (dir == NULL) {
        fprintf(stderr, "Error at opening directory\n");
        return -1;
    }
    strcpy(old_text, text);
    simple_text(text);
    while ((entry = readdir(dir)) != NULL) {
        if (conformity(entry->d_name, text)) {
            printf("conformity %d: ", ++count);
            puts(entry->d_name);
            bool = 1;
        }
    }
    if (bool == 0)
        printf("Not found: % \n", old_text);
    return 0;
}
