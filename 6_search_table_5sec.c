#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <zconf.h>
#include <stdlib.h>
#include <sys/time.h>

#define MAX_BUFF 200
#define MAX_LINES 50

char buffer[MAX_BUFF];

typedef struct Line_entry {
    off_t offset;
    unsigned int length;
} Line_entry;

int build_table(int fd, Line_entry *table, unsigned int max_size) {
    int current_line = 1;
    int current_position = 0;
    table[1].offset = 0L;
    while (true) {
        int read_amount = read(fd, buffer, MAX_BUFF);
        if (read_amount == 0) {
            fprintf(stderr, "End of file\n");
            break;
        }
        if (read_amount == -1) {
            if (errno == EINTR ||
                errno == EAGAIN) //EINTR - Interrupted function call //EAGAIN - The resource is temporarily unavailable
                continue;
            else {
                fprintf(stderr, "Error at building_table() in READ():");
                exit(EXIT_FAILURE);
            }
        }
        for (unsigned int i = 0; i < read_amount; i++) {
            current_position++;
            if (buffer[i] == '\n') {
                table[current_line].length = current_position - 1;
                if (current_line == max_size) {
                    fprintf(stderr, "Long file, wait while read %d lines\n");
                    return current_line - 1;
                }
                table[current_line + 1].offset = table[current_line].offset + current_position;
                current_line++;
                current_position = 0;
            }
        }
    }
    table[current_line].length = current_position;
    return current_line;
}

void print_line(int fd, Line_entry line_entry) {
    lseek(fd, line_entry.offset, SEEK_SET);
    unsigned int bytes_read;
    for (unsigned int i = 0; i < line_entry.length; i += MAX_BUFF) {
        if (MAX_BUFF > line_entry.length - i)
            bytes_read = line_entry.length - i;
        else
            bytes_read = MAX_BUFF;
        if (read(fd, buffer, bytes_read) == -1) {
            fprintf(stderr, "Error at print_line() READ(): ");
            exit(EXIT_FAILURE);
        }
        if (write(STDOUT_FILENO, buffer, bytes_read) == -1) {
            fprintf(stderr, "Error at print_line() WRITE(): ");
            exit(EXIT_FAILURE);
        }
    }
}

int print_table(int fd, Line_entry *table, unsigned int table_size) {
    printf("Lines amount: [%d,%d]. Print number of line within 5 seconds\n", 1, table_size - 1);
    int scan_line = 1;
    fd_set rfds;
    struct timeval timer;
    int retval;
    FD_SET(fd, &rfds);
    timer.tv_sec = 5;
    timer.tv_usec = 0;
    select(2, &rfds, NULL, NULL, &timer);
    if (FD_ISSET(STDIN_FILENO, &rfds)) {
        while (true) {
            printf("print line number: ");
            scanf("%d", &scan_line);
            if (scan_line < 0 || scan_line > table_size) {
                fprintf(stderr, "This line doesn't exist, try again\n");
                continue;
            }
            if (scan_line == 0)
                break;
            print_line(fd, table[scan_line]);
            putchar('\n');
        }
    } else {
        printf("Time is up! Keep the whole file:\n");
        for (unsigned int i = 1; i < table_size; i++) {
            print_line(fd, table[i]);
            putchar('\n');
        }
        exit(EXIT_SUCCESS);
    }
    close(fd);
    if (close(fd) == -1) {
        fprintf(stderr, "Close error\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "%s \n", argv[0]);
        return EINVAL;
    }
    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "File doesn't open");
        return EIO;
    }
    Line_entry *table = malloc(sizeof(Line_entry) * (MAX_LINES + 1));
    int table_building = build_table(fd, table, MAX_LINES);
    int out = print_table(fd, table, table_building);
    free(table);
    return out;
}
