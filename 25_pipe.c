#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    int status1, status2;
    int pipefd[2];
    char buf[100];
    if (argc != 2) {
        fprintf(stderr, "Usage: \n%s <text>\n", argv[0]);
        return -1;
    }
    if (pipe(pipefd) == -1) {
        fprintf(stderr, "Error at pipe");
        return -1;
    }

    pid_t pid1 = fork();
    if (pid1 == -1) {
        fprintf(stderr, "Error at fork()");
        if (close(pipefd[0]) == -1)
            fprintf(stderr, "Error at close pipefd[0]");
        if (close(pipefd[1]) == -1)
            fprintf(stderr, "Error at close pipefd[1]");
        return -1;
    }
    if (pid1 == 0) {
        if (close(pipefd[0]) == -1)
            fprintf(stderr, "Error at close pipefd[0]");
        strcpy(buf, argv[1]);
        if (write(pipefd[1], buf, strlen(buf) + 1) == -1) {
            fprintf(stderr, "Error at write");
            if (close(pipefd[1]) == -1)
                fprintf(stderr, "Error at close pipefd[1]");
            return -1;
        }
    }

    pid_t pid2 = fork();
    if (pid2 == -1) {
        fprintf(stderr, "Error at fork()");
        if (close(pipefd[0]) == -1)
            fprintf(stderr, "Error at close pipefd[0]");
        if (close(pipefd[1]) == -1)
            fprintf(stderr, "Error at close pipefd[1]");
        return -1;
    }
    if (pid2 == 0) {
        if (close(pipefd[1]) == -1)
            fprintf(stderr, "Error at close pipefd[1]");
        if (read(pipefd[0], buf, 100) == -1) {
            fprintf(stderr, "Error at read");
            if (close(pipefd[0]) == -1)
                fprintf(stderr, "Error at close pipefd[0]");
            return -1;
        }
        for (int i = 0; i < strlen(buf); i++)
            buf[i] = (char) toupper(buf[i]);
        printf("Upper text: %s\n", buf);
        if (close(pipefd[0]) == -1) {
            fprintf(stderr, "Error at close pipefd[0]");
            return -1;
        }

    } else {
        waitpid(pid1, &status1, 0);
        waitpid(pid2, &status2, 0);
        if (close(pipefd[0]) == -1) {
            fprintf(stderr, "Error at close pipefd[0]\n");
            return -1;
        }
        if (close(pipefd[1]) == -1) {
            fprintf(stderr, "Error at close pipefd[1]\n");
            return -1;
        }
        return 0;
    }
}
