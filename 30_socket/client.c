#include <sys/socket.h>
#include <string.h>
#include <sys/un.h>
#include <zconf.h>
#include <stdio.h>

#define TEXT "Hello EveRYoNe!"

int main(int argc, char *argv[]) {
    int sock;
    int addr_len;

    struct sockaddr_un name;

    if (argc < 2) {
        printf("Usage: %s <pathname>\n", argv[0]);
        return -1;
    }

    if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Error at opening socket\n");
        return -1;
    }

    name.sun_family = AF_UNIX;
    strcpy(name.sun_path, argv[1]);
    addr_len = sizeof(name.sun_family) + strlen(name.sun_path);

    if (connect(sock, (struct sockaddr *) &name, addr_len) < 0)
        fprintf(stderr, "Error at connect()\n");

    if (write(sock, TEXT, sizeof(TEXT)) != strlen(TEXT))
        fprintf(stderr, "Error at write()\n");

    if (close(sock) == -1)
        fprintf(stderr, "Error at close socket\n");
}
