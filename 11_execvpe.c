#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int execvpe(const char *file, char *const argv[], char *const envp[]) {
    if (clearenv() != 0) {
        fprintf(stderr, "Error at clearenv\n");
        return -1;
    }
    for (int i = 0; envp[i]; ++i) {
        if (putenv(envp[i]) != 0) {
            fprintf(stderr, "Error at putenv\n");
            return -1;
        }
    }
    return execvp(file, argv);
}

int main() {
    char *file = "/bin/sh";
    char *argv[] = {file, "-c", "env", NULL};
    char *envp[] = {"PATH=/usr/bin", "KEY=VALUE", NULL};
    if (execvpe(file, argv, envp) == -1) {
        fprintf(stderr, "Error at execvpe\n");
    }
    return -1;
}
