#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define TEXT "Hello EVERYONE!!!\n"

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_un server;

    if (argc < 2) {
        printf("usage:%s <pathname>", argv[0]);
        exit(1);
    }

    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        fprintf(stderr, "Error at opening stream socket");
        return -1;
    }

    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, argv[1]);

    if (connect(sock, (struct sockaddr *) &server, sizeof(struct sockaddr_un)) < 0) {
        fprintf(stderr, "Error at connecting stream socket");
        if (close(sock) == -1)
            fprintf(stderr, "Error at closing socket\n");
        return -1;
    }

    if (write(sock, TEXT, sizeof(TEXT)) != strlen(TEXT)) {
        fprintf(stderr, "Error at writing TEXT");
        if (close(sock) == -1)
            fprintf(stderr, "Error at closing socket\n");
        return -1;
    }
    
    if (close(sock) == -1)
        fprintf(stderr, "Error at closing socket\n");
}
