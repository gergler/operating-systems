#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#define BUF_SIZE 100

int pipe_close(FILE *pipe) {
    int status = pclose(pipe);
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

int main(int argc, char *argv[]) {
    int status;
    char buf[BUF_SIZE];
    FILE *pipe = popen("echo text", "r");
    if (pipe == NULL) {
        fprintf(stderr, "Error at pipe");
        return -1;
    }
    unsigned int read_bytes = fread(buf, 1, BUFSIZ, pipe);
    if (read_bytes == -1) {
        fprintf(stderr, "Error at fread");
        pipe_close(pipe);
    }
    pipe_close(pipe);
    for (int i = 0; i < read_bytes; i++)
        buf[i] = (char) toupper(buf[i]);
    if (fwrite(buf, read_bytes, 1, stdout) == -1) {
        fprintf(stderr, "Error at fwrite");
        return -1;
    }
    return 0;
}
