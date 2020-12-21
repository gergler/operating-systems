#include <string.h>
#include <sys/time.h>
#include <zconf.h>
#include <ctype.h>

#define SOCKETNAME  "socket_server"

int main() {
    int sock, msg_sock;
    int addr_len;

    int read_num;
    char buf[1024];

    int maxfd, ready_fd;
    fd_set fds;

    struct sockaddr_un name;

    unlink(SOCKETNAME);

    if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Error at opening socket\n");
        return -1;
    }
    memset(&name, 0, sizeof(struct sockaddr_un));
    name.sun_family = AF_UNIX;
    strcpy(name.sun_path, SOCKETNAME);
    addr_len = sizeof(name.sun_family) + strlen(name.sun_path);

    if (bind(sock, (struct sockaddr *) &name, addr_len) < 0) {
        fprintf(stderr, "Error at bind()\n");
        return -1;
    }

    printf("Socket name: %s\n", name.sun_path);

    if (listen(sock, 5) < 0) {
        fprintf(stderr, "Error at listen()\n");
        return -1;
    }

    if ((msg_sock = accept(sock, 0, 0) < -1))
        fprintf(stderr, "Error at accept() msg_socket\n");
    maxfd = msg_sock + 1;
    while (1) {
        FD_ZERO(&fds);
        FD_SET(msg_sock, &fds);

        ready_fd = select(maxfd, &fds, (fd_set *) 0, (fd_set *) 0, (struct timeval *) 0);

        if (ready_fd > 0) {
            if (FD_ISSET(msg_sock, &fds) != 0) {
                read_num = read(msg_sock, buf, sizeof(buf));
                if (read_num < 1) {
                    fprintf(stderr, "Can't recv\n");
                    break;
                }
                if (read_num == 0) {
                    printf("End connection\n");
                    break;
                } else {
                    for (int i = 0; i < strlen(buf); i++)
                        buf[i] = (char) toupper(buf[i]);
                }
            } else {
                printf("Socket absent in fd_set\n");
            }
            if (ready_fd == -1) {
                fprintf(stderr, "Error at select()\n");
                break;
            }
        }
        if (close(msg_sock) == -1) {
            fprintf(stderr, "Error at close msg_socket\n");
            break;
        }
    }
    if (close(sock) == -1)
        fprintf(stderr, "Error at close socket\n");
}
