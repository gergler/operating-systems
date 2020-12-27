#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <memory.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>

void print_status(struct stat *status) {
    switch (status->st_mode & S_IFMT) { //маска для типа файла
        case S_IFREG:
            printf("-");
            break;
        case S_IFDIR:
            printf("d");
            break;
        default:
            printf("?");
    };
}

void print_access(struct stat *status) {
    printf(status->st_mode & S_IRUSR ? "r" : "-");
    printf(status->st_mode & S_IWUSR ? "w" : "-");
    printf(status->st_mode & S_IXUSR ? "x" : "-");
    printf(status->st_mode & S_IRGRP ? "r" : "-");
    printf(status->st_mode & S_IWGRP ? "w" : "-");
    printf(status->st_mode & S_IXGRP ? "x" : "-");
    printf(status->st_mode & S_IROTH ? "r" : "-");
    printf(status->st_mode & S_IWOTH ? "w" : "-");
    printf(status->st_mode & S_IXOTH ? "x" : "-");
    printf("\t");
}

int print_info(struct stat *status) {
    struct passwd *pwd = getpwuid(status->st_uid);
    if (pwd == NULL) {
        fprintf(stderr, "Error at getpwuid\n");
        return -1;
    }
    printf(" %10s", pwd->pw_name);
    struct group *grp = getgrgid(status->st_gid);
    if (grp == NULL) {
        fprintf(stderr, "Error at getgrgid\n");
        return -1;
    }
    printf(" %10s", grp->gr_name);
    return 0;
}

int main(int argc, char *argv[]) {
    struct dirent *entry;
    DIR *dir = opendir("./");
    struct stat status;
    struct tm *time;

    if (dir == NULL) {
        fprintf(stderr, "Error at opening directory\n");
        return -1;
    }
    while ((entry = readdir(dir)) != NULL) {
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
            continue;
        for (int i = 0; i < argc; ++i) {
            if (!strcmp(entry->d_name, argv[i])) {
                if (stat(entry->d_name, &status) == -1) {
                    fprintf(stderr, "Error at stat\n");
                    return -1;
                }
                print_status(&status);
                print_access(&status);
                printf("%2ld\t", status.st_nlink);
                if (print_info(&status) != 0) {
                    fprintf(stderr, "Error at function: print_info()\n");
                    return -1;
                }
                if (status.st_mode & S_IFDIR)
                    printf("%10ld\t", status.st_size);
                else
                    printf("%10c\t", ' ');
                time = localtime(&status.st_mtime);
                printf("%d:%d %d.%d.%d\t", time->tm_hour,
                       time->tm_min, time->tm_mday, time->tm_mon + 1,
                       time->tm_year + 1900);
                printf("%s\n", entry->d_name);
                break;
            }
        }
    }
    if (closedir(dir) == -1) {
        fprintf(stderr, "Error at closedir\n");
        return -1;
    }
    return 0;
}
