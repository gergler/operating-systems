#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char **argv){
    printf("real UID: %d\t", getuid());
    printf("real EUID: %d\n", geteuid());

    if (argc != 2) {
        fprintf(stderr, "No file\n");
        return EXIT_FAILURE;
    }

    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("File didn't open");
    }

    fclose(file);

    uid_t uid = geteuid();
    setuid(uid);
    printf("UID: %d\t", getuid());
    printf("EUID: %d\n", geteuid());

    file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("File didn't open");
    }

    fclose(file);

    return EXIT_SUCCESS;
}
