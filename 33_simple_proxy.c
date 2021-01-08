#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>

#define BUF_SIZE 4096
#define LISTEN  10
#define PORT "5000"
#define LISTEN_PORT "5000"

static void sigaction_handler(int sig) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

unsigned int transfer(int from, int to) {
    char buf[BUF_SIZE];
    unsigned int disconnected = 0;
    size_t bytes_read, bytes_written;
    bytes_read = read(from, buf, BUF_SIZE);
    if (bytes_read == 0) {
        disconnected = 1;
    } else {
        bytes_written = write(to, buf, bytes_read);
        if (bytes_written == -1) {
            disconnected = 1;
        }
    }
    return disconnected;
}

void handle(int client) {
    struct addrinfo hints, *res;
    int server = -1;
    unsigned int disconnected = 0;
    fd_set set;
    unsigned int max_sock;

    sigset_t mask, old_mask;
    struct sigaction act;

    memset (&act, 0, sizeof(act));
    act.sa_handler = sigaction_handler;
    act.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &act, NULL) == -1) {
        fprintf(stderr, "Error at sigaction()\n");
        exit(-1);
    }
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    if (sigprocmask(SIG_BLOCK, &mask, &old_mask) < 0) {
        fprintf(stderr, "Error at sigprocmask()\n");
        exit(-1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo("ar-net.mykeenetic.net", PORT, &hints, &res) != 0) {
        fprintf(stderr, "Error at getaddrinfo()\n");
        close(client);
    }

    server = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (server == -1) {
        fprintf(stderr,"Error at socket()\n");
        close(client);
    }

    if (connect(server, res->ai_addr, res->ai_addrlen) == -1) {
        fprintf(stderr, "Error at connect()\n");
        close(client);
    }

    if (client > server) {
        max_sock = client;
    } else {
        max_sock = server;
    }

    while (!disconnected) {
        FD_ZERO(&set);
        FD_SET(client, &set);
        FD_SET(server, &set);
        if (pselect(max_sock + 1, &set, NULL, NULL, NULL, &old_mask) == -1) {
            fprintf(stderr, "Error at select()\n");
            break;
        }
        if (FD_ISSET(client, &set)) {
            disconnected = transfer(client, server);
        }
        if (FD_ISSET(server, &set)) {
            disconnected = transfer(server, client);
        }
    }
    close(server);
    close(client);
}

int main() {
    int sock;
    struct addrinfo hints, *res;
    int reuseaddr = 1;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(NULL, LISTEN_PORT, &hints, &res) != 0) {
        fprintf(stderr, "Error at getaddrinfo()\n");
        return -1;
    }

    sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock == -1) {
        fprintf(stderr, "Error at socket()\n");
        freeaddrinfo(res);
        return -1;
    }

    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(int)) == -1) {
        fprintf(stderr, "Error at setsockopt()\n");
        freeaddrinfo(res);
        return -1;
    }

    if (bind(sock, res->ai_addr, res->ai_addrlen) == -1) {
        fprintf(stderr, "Error at bind()\n");
        freeaddrinfo(res);
        return -1;
    }

    if (listen(sock, LISTEN) == -1) {
        fprintf(stderr, "Error at listen()\n");
        freeaddrinfo(res);
        return -1;
    }

    freeaddrinfo(res);

    while (1) {
        struct sockaddr_in their_addr;
        socklen_t size = sizeof(struct sockaddr_in);
        int newsock = accept(sock, (struct sockaddr *) &their_addr, &size);
        if (newsock == -1) {
            fprintf(stderr, "Error at accept()\n");
            return -1;
        } else {
            printf("Got a connection from %s on port %d\n",
                   inet_ntoa(their_addr.sin_addr), htons(their_addr.sin_port));
        }
        int pid = fork();
        if (pid == 0) {
            close(sock);
            handle(newsock);
            return 0;
        } else {
            if (pid == -1) {
                fprintf(stderr, "Error at fork()\n");
                return -1;
            } else {
                close(newsock);
            }
        }
    }
    close(sock);
    return 0;
}
