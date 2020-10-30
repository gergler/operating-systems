#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <zconf.h>
#include <stdlib.h>

#define MAX_SIZE 200
#define MAX_LINES 100

char buffer[MAX_SIZE];

typedef struct Line_entry {
    off_t offset;
    unsigned int length;
} Line_entry;

int build_table(int open_id, Line_entry *table, unsigned int max_size) {
    int current_line = 1;
    int current_position = 0;
    table[1].offset = 0L;
    while (true) {
        int read_amount = read(open_id, buffer, MAX_SIZE);
        if (read_amount == 0)
            break;
        if (read_amount == -1) {
            perror("Error at building_table() read(): ");
            exit(EXIT_FAILURE);
        }
        for (unsigned int i = 0; i < read_amount; i++) {
            current_position++;
            if (buffer[i] == '\n') {
                table[current_line].length = current_position - 1;
                if (current_line == max_size) {
                    printf("Long file, wait while read %d lines\n", max_size);
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

void print_line(int open_id, Line_entry line_entry) {
    lseek(open_id, line_entry.offset, SEEK_SET);
    unsigned int bytes_read;
    for (unsigned int i = 0; i < line_entry.length; i += MAX_SIZE) {
        if (MAX_SIZE > line_entry.length - i)
            bytes_read = line_entry.length - i;
        else
            bytes_read = MAX_SIZE;
        if (read(open_id, buffer, bytes_read) == -1) {
            perror("Error at print_line() read(): ");
            exit(EXIT_FAILURE);
        }
        if (write(STDOUT_FILENO, buffer, bytes_read) == -1) {
            perror("Error at print_line() write(): ");
            exit(EXIT_FAILURE);
        }
    }
}

int print_table(int open_id, Line_entry *table, unsigned int table_size) {
    while (true) {
        int line, result;
        char end;
        printf("$ ");
        result = scanf("%d%c", &line, &end);
        if (result != 2 || end != '\n') {
            fprintf(stderr, "Format error\n");
            return EXIT_FAILURE;
        }
        if (line == '\0')
            break;
        if (line < 0 || line > table_size) {
            fprintf(stderr, "Out of range error\n");
            continue;
        }
        print_line(open_id, table[line]);
        putchar('\n');
    }
    return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "%s \n", argv[0]);
        return EINVAL;
    }
    int open_id = open(argv[1], O_RDONLY);
    if (open_id == -1) {
        perror(argv[0]);
        return EIO;
    }
    Line_entry *table = malloc(sizeof(Line_entry) * (MAX_LINES + 1));
    int table_size = build_table(open_id, table, MAX_LINES);
    int out = print_table(open_id, table, table_size);
    free(table);
    return out;
}
