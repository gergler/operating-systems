#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    int status;
    if (argc != 2) {
        fprintf(stderr, "No file\n");
        return EINVAL;
    }
    pid_t pid = fork();
    if (pid == -1) {
        fprintf(stderr, "Error at fork()");
        return -1;
    }
    if (pid == 0) {
        execl("/bin/cat", "cat", argv[1], NULL);
        _exit(-1);
    }
    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
        printf("Child process end\n");
        return 0;
    } else {
        fprintf(stderr, "Error at terminating child process\n");
        return -1;
    }
}
