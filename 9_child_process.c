#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    int status;
    if (argc < 2) {
        fprintf(stderr, "Usage:\n%s file_name\n", argv[0]);
        return EINVAL;
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
        printf("Child ended okay\n, ");
        return 0;
    }
    if (WIFSIGNALED(status)) {
        printf("Child ended due to signal\n");
        return 0;
    }
}
