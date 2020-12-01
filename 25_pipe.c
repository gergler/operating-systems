#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>

#define BUF_SIZE 100

int main(int argc, char *argv[]) {
    int status1, status2;
    int pipefd[2];
    char buf[BUF_SIZE];
    if (argc != 2) {
        fprintf(stderr, "Usage: \n%s <text>\n", argv[0]);
        return -1;
    }
    if (pipe(pipefd) == -1) {
        fprintf(stderr, "Error at pipe\n");
        return -1;
    }

    pid_t pid1 = fork();
    if (pid1 == -1) {
        fprintf(stderr, "Error at fork() 1\n");
        if (close(pipefd[0]) == -1)
            fprintf(stderr, "Error at close pipefd[0] 1\n");
        if (close(pipefd[1]) == -1)
            fprintf(stderr, "Error at close pipefd[1] 1\n");
        return -1;
    }
    if (pid1 == 0) {
        if (close(pipefd[0]) == -1)
            fprintf(stderr, "Error at close pipefd[0] 1\n");
        if (write(pipefd[1], argv[1], strlen(argv[1])) != strlen(argv[1])) {
            fprintf(stderr, "Can't write all write\n");
            if (close(pipefd[1]) == -1)
                fprintf(stderr, "Error at close pipefd[1] 1\n");
            return -1;
        }
        if (close(pipefd[1]) == -1)
            fprintf(stderr, "Error at close pipefd[1] 1\n");
        return -1;
    } else {
        pid_t pid2 = fork();
        if (pid2 == -1) {
            fprintf(stderr, "Error at fork() 2\n");
            if (close(pipefd[0]) == -1)
                fprintf(stderr, "Error at close pipefd[0] 2\n");
            if (close(pipefd[1]) == -1)
                fprintf(stderr, "Error at close pipefd[1] 2\n");
            waitpid(pid1, &status1, 0);
            return -1;
        }
        if (pid2 == 0) {
            if (close(pipefd[1]) == -1)
                fprintf(stderr, "Error at close pipefd[1] 2\n");
            if (read(pipefd[0], buf, strlen(argv[1])) < 0) {
                fprintf(stderr, "Can't read string\n");
                if (close(pipefd[0]) == -1)
                    fprintf(stderr, "Error at close pipefd[0] 2\n");
                return -1;
            }
            for (int i = 0; i < strlen(buf); i++)
                buf[i] = (char) toupper(buf[i]);
            printf("Upper text: %s\n", buf);
            if (close(pipefd[0]) == -1) {
                fprintf(stderr, "Error at close pipefd[0] 2\n");
                return -1;
            }

        } else {
            waitpid(pid1, &status1, 0);
            waitpid(pid2, &status2, 0);
            if (close(pipefd[0]) == -1) {
                fprintf(stderr, "Error at close pipefd[0] P\n");
                return -1;
            }
            if (close(pipefd[1]) == -1) {
                fprintf(stderr, "Error at close pipefd[1] P\n");
                return -1;
            }
        }
    }
    return 0;
}
