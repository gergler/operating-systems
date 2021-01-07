#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>

#define MAX_FD 256
#define MAX_BUF 256

int main(int argc, char **argv) {
    FILE *fd[MAX_FD];
    char buf[MAX_BUF];

    fd_set fds;
    int fd_num = 0;

    struct timeval timeout;

    for (int i = 1; i < argc; i++) {
        fd[fd_num] = fopen(argv[i], "rt");
        if (fd[fd_num] == NULL)
            fprintf(stderr, "Error at %s\n", argv[i]);
        if (++fd_num == MAX_FD)
            break;
    }

    while (fd_num) {
        for (int i = 0; i < fd_num; i++) {
            timeout.tv_sec = 1;
            timeout.tv_usec = 0;
            FD_ZERO(&fds);
            FD_SET(fileno(fd[i]), &fds);
            int fd_amount = select(fileno(fd[i]) + 1, &fds, NULL, NULL, &timeout);
            if (fd_amount > 0) {
                if (fgets(buf, MAX_BUF, fd[i]) != 0)
                    write(1, buf, strlen(buf));
                else {
                    close(fileno(fd[i]));
                    fd[i--] = fd[--fd_num];
                }
            }
            if (fd_amount == -1) {
                fprintf(stderr, "Error at select()\n");
                return -1;
            }
        }
    }

    return 0;
}
