#include <stdio.h>
#include <signal.h>
#include <termio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>

struct termio savetty;

void sigcatch() {
    ioctl(0, TCSETAF, &savetty);
    exit(-1);
}

int main() {
    struct termio newtty;
    signal(SIGINT, sigcatch);
    if (ioctl(STDIN_FILENO, TCGETA, &savetty) == -1) {
        printf("ioctl failed: no terminal\n");
        exit(-1);
    }
    newtty = savetty;
    newtty.c_lflag &= ~ICANON;/* выход из канонического режима */
    newtty.c_lflag &= ~ECHO;  /* отключение эхо-сопровождения*/
    newtty.c_cc[VMIN] = 1;    /* минимум 5 символов */
    if (ioctl(STDIN_FILENO, TCSETAF, &newtty) == -1) {
        fprintf(stderr, "Can not switch the terminal to unprocessed mode\n");
        exit(-1);
    }
    printf("(Y)es or (N)o?\n");
    while (1) {
        char buf;
        if (read(STDIN_FILENO, &buf, 1) == 1) {
            if (tolower(buf) == 'y') {
                printf("U choose: Yes\n");
                sleep(1000);
            } else if (tolower(buf) == 'n') {
                printf("U choose: No\n");
                sleep(1000);
            } else {
                write(STDOUT_FILENO, "\a", 1);
            }
        } else {
            fprintf(stderr, "Couldn't read from standard input");
        }
    }
}
