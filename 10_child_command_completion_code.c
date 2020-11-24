#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    int status;
    if (argc < 2) {
        fprintf(stderr, "Usage:\n%s executable_arguments\n", argv[0]);
        return -1;
    }
    pid_t pid = fork();
    if (pid == -1) {
        fprintf(stderr, "Error at fork()");
        return -1;
    }
    if (pid == 0)
        execvp(argv[1], &argv[1]);
    waitpid(pid, &status, 0);
    if (WIFSIGNALED(status)) {
        printf("Child ended due to signal: %d\tchild pid: %d\n", WTERMSIG(status), pid);
        return 0;
    }
    if (WIFEXITED(status)) {
        printf("Child process ended, child pid: %d\tdescendant exit code: %d\n", pid, WEXITSTATUS(status));
        return 0;
    }
    return 0;
}
