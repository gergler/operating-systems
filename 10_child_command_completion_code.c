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
        execl("/bin/cat", "cat", argv[1], NULL);
    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
        printf("Child process ended, child pid: %d\tdescendant exit code: %d\n", pid, WEXITSTATUS(status));
        return 0;
    }
    if (WIFSIGNALED(status)) {
        printf("Child ended due to signal: %d\n", WTERMSIG(status));
        return 0;
    }
}
