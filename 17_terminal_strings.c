#include <stdio.h>
#include <ctype.h>
#include <termios.h>
#include <unistd.h>

#define STRING_SIZE 40

typedef struct {
    char data[STRING_SIZE];
    size_t size;
} string;

void erase(string *str, size_t count) {
    str->size -= count;
    for (size_t i = 0; i < count; ++i)
        write(STDOUT_FILENO, "\b \b", 3);
}

void newline(string *str) {
    write(STDOUT_FILENO, "\n", 1);
    str->size = 0;
}

void erase_word(string *str) {
    size_t count = 0;
    for (count = str->size - 1; count >= 0; --count) /*spaces*/
        if (isspace(str->data[count]))
            break;

    for (; count >= 0; --count) /*word*/
        if (!isspace(str->data[count]))
            break;

    erase(str, str->size - count - 1);
}

void transfer(string *str) {
    size_t count, old_size = str->size;
    for (count = old_size - 1; count >= 0; --count)
        if (isspace(str->data[count]))
            break;

    if (count >= 0) {
        erase(str, str->size - ++count);
        newline(str);
        while (count++ < old_size) {
            write(STDOUT_FILENO, &(str->data[count]), 1);
            str->data[str->size++] = str->data[count];
        }
    } else {
        newline(str);
    }
}

int main() {
    if (isatty(STDIN_FILENO) == 0) {
        fprintf(stderr, "Standard input is not a terminal");
        return -1;
    }
    struct termios attributes;
    struct termios old_attributes;
    if (tcgetattr(STDIN_FILENO, &attributes) == -1) {
        fprintf(stderr,"Couldn't get terminal attributes");
        return -1;
    }
    old_attributes = attributes;
    attributes.c_lflag &= ~ICANON;
    attributes.c_lflag &= ~ECHO;
    attributes.c_cc[VMIN] = 1;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &attributes) == -1) {
        fprintf(stderr,"Couldn't set terminal attributes");
        return -1;
    }

    string str;
    str.size = 0;
    while (1) {
        char buf;
        if (read(STDIN_FILENO, &buf, 1) == 1) {
            if (isprint(buf)) {
                if (str.size == STRING_SIZE) {
                    if (isspace(buf)) {
                        newline(&str);
                    } else {
                        transfer(&str);
                    }
                }
                write(STDOUT_FILENO, &buf, 1);
                str.data[str.size++] = buf;
            } else if ((attributes.c_cc[VERASE] == buf) && (str.size)) {
                erase(&str, 1);
            } else if (attributes.c_cc[VKILL] == buf) {
                erase(&str, str.size);
            } else if (attributes.c_cc[VWERASE] == buf) {
                erase_word(&str);
            } else if (attributes.c_cc[VEOF] == buf && !str.size) {
                break;
            } else if ('\n' == buf) {
                newline(&str);
            } else {
                write(STDOUT_FILENO, "\a", 1);
            }
        } else {
            fprintf(stderr,"Couldn't read from standard input");
        }
    }

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_attributes) == -1) {
        fprintf(stderr,"Couldn't restore terminal attributes");
        return -1;
    }
    return 0;
}
