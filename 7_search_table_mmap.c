#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <zconf.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define MAX_LINES 50

typedef struct Line_entry {
    off_t offset;
    unsigned int length;
} Line_entry;

int build_table(int fd, Line_entry *table, char *map, off_t file_size) {
    int line = 1;
    int current_position = 0;
    table[1].offset = 0L;
    for (off_t position = 0L; position < file_size; position++) {
        current_position++;
        if (map[position] == '\n') {
            table[line].length = current_position;
            table[line + 1].offset = position + 1;
            current_position = 0;
            line++;
        }
    }
    table[line].length = current_position;
    if (munmap(map, file_size) == -1) {
        close(fd);
        if (close(fd) == -1) {
            fprintf(stderr, "Error at close");
            return -1;
        }
        fprintf(stderr, "Error un-mmapping the file");
        return -1;
    }
    return line;
}

void print_line(Line_entry line_entry, char *map) {
    for (unsigned int i = 0; i < line_entry.length; i++)
        printf("%c", map[line_entry.offset + i]);
}

int print_table(int fd, Line_entry *table, char *map, unsigned int table_size) {
    printf("\nLines amount: [%d,%d]. Print number of line within 5 seconds \n", 1, table_size - 1);
    int scan_line;
    int max_fd = 0;
    fd_set rfds;
    struct timeval timeout;
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    int fd_amount = select(max_fd + 1, &rfds, NULL, NULL, &timeout);
    if (FD_ISSET(fd, &rfds) != 0) {
        if (fd_amount > 0) {
            while (true) {
                printf("print line number: ");
                scanf("%d", &scan_line);
                if (scan_line < 0 || scan_line > table_size - 1) {
                    fprintf(stderr, "This line doesn't exist, try again\n");
                    continue;
                }
                if (scan_line == 0)
                    break;
                print_line(table[scan_line], map);
                putchar('\n');
            }
        }
        if (fd_amount == -1) {
            fprintf(stderr, "Error at select()\n");
            return -1;
        }
        if (fd_amount == 0) {
            printf("Time is up! Keep the whole file:\n");
            for (unsigned int i = 1; i < table_size; i++) {
                print_line(table[i], map);
                putchar('\n');
            }
        }
        close(fd);
        if (close(fd) == -1) {
            fprintf(stderr, "Close error\n");
            return -1;
        }
    } else {
        printf("fd absent in fd_set");
    }
    return 0;
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
    if (table == NULL) {
        fprintf(stderr, "Failled to allocate memory\n");
        return -1;
    }

    struct stat file_info;
    if (fstat(fd, &file_info) == -1) {
        fprintf(stderr, "Error getting the file size");
        return -1;
    }
    if (file_info.st_size == 0) {
        fprintf(stderr, "Error: File is empty, nothing to do\n");
        return -1;
    }
    char *map = mmap(0, file_info.st_size, PROT_READ, MAP_SHARED, fd, 0L);

    int table_building = build_table(fd, table, map, file_info.st_size);
    if (table_building == -1) {
        fprintf(stderr, "Error at table_building");
        free(table);
        return -1;
    }
    int out = print_table(fd, table, map, table_building);
    free(table);
    if (map == MAP_FAILED) {
        close(fd);
        if (close(fd) == -1) {
            fprintf(stderr, "Error at close");
            return -1;
        }
        fprintf(stderr, "Error mmapping the file");
        return -1;
    }
    return out;
}
