#include <stdio.h>
#include <string.h>
#include <dirent.h>

#define MAX_SIZE 200

void simple_text(char *const pattern) {
    int count = 1;
    size_t length = strlen(pattern);
    for (int i = 0; i < length; i++)
        if ('*' != pattern[i - 1] || '*' != pattern[i])
            pattern[count++] = pattern[i];
    pattern[count] = 0;
}

int exist(const int i, const int j) {
    return (i >= 0 && j >= 0);
}

int conformity(const char *word, const char *pattern) {
    int array[MAX_SIZE][MAX_SIZE];
    size_t word_len = strlen(word);
    size_t pattern_len = strlen(pattern);
    for (int i = 0; i < word_len; i++)
        for (int j = 0; j < pattern_len; j++)
            array[i][j] = 0;
    array[0][0] = (pattern[0] == '*' || pattern[0] == '?' || pattern[0] == word[0]);
    for (int i = 0; i < word_len; i++) {
        for (int j = 0; j < pattern_len; j++) {
            switch (pattern[j]) {
                case '?':
                    array[i][j] = (exist(i - 1, j - 1) && array[i - 1][j - 1]) || (0 == i && 1 == j && '*' == pattern[j - 1]);
                    break;
                case '*':
                    array[i][j] = (exist(i - 1, j) && array[i - 1][j]) || (exist(i, j - 1) && array[i][j - 1]);
                    break;
                default:
                    array[i][j] = (word[i] == pattern[j])  && ((exist(i - 1, j - 1) && array[i - 1][j - 1]) || (0 == i && 1 == j && '*' == pattern[j - 1]));
                    break;
            }
        }
    }
    return array[word_len - 1][pattern_len -1];
}


int main() {
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char pattern[MAX_SIZE] = {0};
    char old_pattern[MAX_SIZE] = {0};
    int count = 0, bool = 0;
    printf("Pattern: ");
    fgets(pattern, MAX_SIZE, stdin);
    if (pattern[strlen(pattern) - 1] == '\n')
        pattern[strlen(pattern) - 1] = 0;
    if (strchr(pattern, '/') != NULL) {
        fprintf(stderr, "Incorrect, U have '/' \n");
        return -1;
    }
    dir = opendir(".");
    if (dir == NULL) {
        fprintf(stderr, "Error at opening directory\n");
        return -1;
    }
    strcpy(old_pattern, pattern);
    simple_text(pattern);
    while ((entry = readdir(dir)) != NULL) {
        if (conformity(entry->d_name, pattern)) {
            printf("conformity %d: ", ++count);
            puts(entry->d_name);
            bool = 1;
        }
    }
    if (bool == 0)
        printf("Not found: %s \n", old_pattern);
    return 0;
}

