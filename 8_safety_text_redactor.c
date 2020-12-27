#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <zconf.h>

int main(int argc, char* argv[]) {
    struct flock lock;
    char text[32];
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file_name>\n", argv[0]);
        return EINVAL;
    }
    int fd = open(argv[1], O_RDWR);
    if (fd == -1) {
        fprintf(stderr, "File doesn't open\n");
        return -1;
    }
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    if (fcntl(fd, F_SETLK, &lock) == -1) {
        if ((errno == EACCES) || (errno == EAGAIN)) {
            printf("Right now %s busy, try again later...\n", argv[1]);
            return -1;
        }
        fprintf(stderr, "Error at %s", argv[1]);
        return -1;
    }
    sprintf(text, "nano %s\n", argv[1]);
    system(text);
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);
    return 0;
}
