#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int status;
    char count[3];
    FILE *pipe = popen("grep \"^$\" ./text.txt | wc -l ", "r");
    if (pipe == NULL) {
        fprintf(stderr, "Error at pipe");
        return -1;
    }
    fgets(count, 3, pipe);
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
    printf("count of blank lines: %s\n", count);
    return 0;
}
