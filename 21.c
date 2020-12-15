#include <stdio.h>
#include <termios.h>
#include <zconf.h>
#include <signal.h>
#include <stdlib.h>

struct termios old_attributes, attributes;
int count;

void sig_catch(int sig) {
    if (sig == SIGQUIT) {
        printf("\nCount of clicks: %d\n", count);
        tcsetattr(STDIN_FILENO, TCSANOW, &old_attributes);
        exit(0);
    }
    if (sig == SIGINT) {
        printf("\a");
        ++count;
        fflush(stdout);
    }
}

int main() {
    if (isatty(STDIN_FILENO) == 0) {
        fprintf(stderr, "Standard input is not a terminal");
        return -1;
    }
    if (tcgetattr(STDIN_FILENO, &attributes) == -1) {
        fprintf(stderr,"Couldn't get terminal attributes");
        return -1;
    }
    old_attributes = attributes;
    attributes.c_lflag &= ~ECHO;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &attributes) == -1) {
        fprintf(stderr, "Couldn't set new terminal attributes");
        return -1;
    }
    while (1) {
        if (signal(SIGINT, sig_catch) == SIG_ERR)
            fprintf(stderr, "SIGINT");
        if (signal(SIGQUIT, sig_catch) == SIG_ERR)
            fprintf(stderr, "SIGQUIT");
    }
}
