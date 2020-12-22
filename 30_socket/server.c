#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>

#define   MSGSIZE   32
#define   NAME   "mysocket30"

int main() {
    int sock, msgsock;
    int read_num;
    struct sockaddr_un server;
    char buf[MSGSIZE];

    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        fprintf(stderr, "Error at opening stream socket\n");
        return -1;
    }

    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, NAME);

    if (bind(sock, (struct sockaddr *) &server, sizeof(struct sockaddr_un))) {
        fprintf(stderr, "Error at binding stream socket\n");
        if (close(sock) == -1)
            fprintf(stderr, "Error at closing socket\n");
        return -1;
    }

    printf("Socket has name %s\n", server.sun_path);
    listen(sock, 5);

    while (1) {
        msgsock = accept(sock, 0, 0);
        if (msgsock == -1)
            fprintf(stderr, "Error at accepting\n");
        else
            while (read_num > 0) {
                if ((read_num = read(msgsock, buf, 1024)) < 0) {
                    fprintf(stderr, "Error at reading stream message\n");
                    break;
                }
                if (read_num == 0) {
                    printf("Ending connection\n");
                    break;
                } else {
                    for (int i = 0; i < MSGSIZE; i++)
                        buf[i] = (char) toupper(buf[i]);
                }
                printf("%s\n", buf);
            }
        if (close(sock) == -1)
            fprintf(stderr, "Error at closing socket\n");
        break;
    }
    if (close(sock) == -1)
        fprintf(stderr, "Error at closing socket\n");
}
