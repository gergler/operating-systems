#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    int pipefd[2];
    char buf[100];
    if (argc != 2) {
        fprintf(stderr, "Usage: \n%s <text>\n", argv[0]);
        return -1;
    }
    if(pipe(pipefd) == -1){
        fprintf(stderr,"Error at pipe");
        return -1;
    }
    pid_t pid = fork();
    if (pid == -1) {
        fprintf(stderr, "Error at fork()");
        close(pipefd[0]);
        close(pipefd[1]);
        return -1;
    }
    if (pid == 0) {
        close(pipefd[1]);
        if (read(pipefd[0], buf, 100) == -1) {
            fprintf(stderr, "Error at read");
            close(pipefd[0]);
            return -1;
        }
        for (int i = 0; i < strlen(buf); i++)
            buf[i] = (char)toupper(buf[i]);
        printf("Upper text: %s\n", buf);
        close(pipefd[0]);
    }
    else {
        close(pipefd[0]);
        strcpy(buf, argv[1]);
        if (write(pipefd[1], buf, strlen(buf) + 1) == -1) {
            fprintf(stderr, "Error at write");
            close(pipefd[1]);
            return -1;
        }
        printf("Text: %s\n", buf);
        close(pipefd[1]);
        wait(NULL);
        return 0;
    }
}
