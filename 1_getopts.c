#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <zconf.h>
#include <sys/resource.h>
#include <errno.h>

#define MAX_DIR 256

void print_ulimit() {
    struct rlimit limit;
    if (getrlimit(RLIMIT_FSIZE, &limit) == 0) {
        if (limit.rlim_cur == RLIM_INFINITY) {
            printf("Ulimit: unlimited\n");
        } else {
            printf("Ulimit: %ld\n", limit.rlim_cur);
        }
    } else {
        fprintf(stderr, "getrlimit");
    }
}

void change_ulimit(char *arg) {
    char *endptr = NULL;
    errno = 0;
    long newLimit = strtol(arg, &endptr, 10);
    if (errno != 0 || *endptr || newLimit < 0) {
        fprintf(stderr, "Invalid new limit value: %s\n", arg);
        return;
    }
    struct rlimit limit;
    if (getrlimit(RLIMIT_FSIZE, &limit) == 0) {
        limit.rlim_cur = (rlim_t) newLimit;
        if (setrlimit(RLIMIT_FSIZE, &limit) != 0)
            fprintf(stderr, "setrlimit\n");
    } else {
        fprintf(stderr, "getrlimit\n");
    }
}

void csize() {
    struct rlimit limit;
    if (getrlimit(RLIMIT_CORE, &limit) == 0) {
        printf("Core file size limit: %ld\n", limit.rlim_cur);
    } else {
        fprintf(stderr, "getrlimit\n");
    }
}

void change_csize(char *arg) {
    char *endptr = NULL;
    errno = 0;
    long newLimit = strtol(arg, &endptr, 10);
    if (errno != 0 || *endptr || newLimit < 0) {
        fprintf(stderr, "Invalid new core limit value: %s\n", arg);
        return;
    }
    struct rlimit limit;
    if (getrlimit(RLIMIT_CORE, &limit) == 0) {
        limit.rlim_cur = (rlim_t) newLimit;
        if (setrlimit(RLIMIT_CORE, &limit) != 0)
            fprintf(stderr, "setrlimit\n");
    } else {
        fprintf(stderr, "getrlimit\n");
    }
}

void dir() {
    char buffer[MAX_DIR];
    char *dir = getcwd(buffer, MAX_DIR);
    if (dir) {
        printf("%s\n", dir);
    } else {
        fprintf(stderr, "getcwd\n");
    }
}

int main(int argc, char *argv[], char *envp[]) {
    int opt;
    while ((opt = getopt(argc, argv, "ispuU:cC:dvV:")) != -1) {
        switch (opt) {
            case 'i':
                printf("Real user ID: %d\n", getuid());
                printf("Effective user ID: %d\n", geteuid());
                printf("Real group ID: %d\n", getgid());
                printf("Effective group ID: %d\n", getegid());
                break;
            case 's':
                if (setpgrp() == -1)
                    fprintf(stderr, "Error at setpgid\n");
                break;
            case 'p':
                printf("Process ID: %d\n", getpid());
                printf("Parent process ID: %d\n", getppid());
                printf("Process group ID: %d\n", getpgrp());
                break;
            case 'U':
                change_ulimit(optarg);
            case 'u':
                print_ulimit();
                break;
            case 'C':
                change_csize(optarg);
            case 'c':
                csize();
                break;
            case 'd':
                dir();
                break;
            case 'V':
                if (putenv(optarg) != 0)
                    fprintf(stderr, "putenv\n");
            case 'v':
                for (; *envp; ++envp)
                    printf("%s\n", *envp);
                break;
            default:
                printf("This command doesn't exist\n");
                return -1;
        }
    }
    return 0;
}
