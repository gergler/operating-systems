#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

int main(int argc, char *argv[]) {
    int status, count;
    FILE *pipe = popen("grep \"^$\" ./text.txt | wc -l ", "r");
    if (pipe == NULL) {
        fprintf(stderr, "Error at pipe");
        return -1;
    }
    if (EOF == fscanf(pipe, "%d", &count)) {
        fprintf(stderr, "Error at scanf");
        status = pclose(pipe);
        if (status == -1) {
            fprintf(stderr, "Error at pipe closing");
            return -1;
        } else if (WEXITSTATUS(status) != 0) {
            fprintf(stderr, "Child process ended with bad exit code: %d\n", WEXITSTATUS(status));
            return -1;
        } else if (WIFSIGNALED(status) != 0) {
            fprintf(stderr, "Child ended due to signal: %d\n", WTERMSIG(status));
            return -1;
        }
    }
    status = pclose(pipe);
    if (status == -1) {
        fprintf(stderr, "Error at pipe closing");
        return -1;
    } else if (WEXITSTATUS(status) != 0) {
        fprintf(stderr, "Child process ended with bad exit code: %d\n", WEXITSTATUS(status));
        return -1;
    } else if (WIFSIGNALED(status) != 0) {
        fprintf(stderr, "Child ended due to signal: %d\n", WTERMSIG(status));
        return -1;
    }
    printf("count of blank lines: %d\n", count);
    return 0;
}
