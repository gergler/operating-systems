#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <libgen.h>

#define NUM 100
#define LINE 10

int main() {
    FILE *fp[2];
    
    if (p2open("/bin/sort -nb", fp) == 0) {
        fprintf(stderr, "Can't make pipe");
        return -1;
    }
    
    srand(time(NULL));
    
    for (int i = 0; i < NUM; ++i) {
        fprintf(fp[0], "%d\n", rand() % NUM);
    }
    
    fclose(fp[0]);
    
    for (int i = 0; i < NUM; i += LINE) {
        for (int j = 0; j < LINE; ++j) {
            int val;
            fscanf(fp[1], "%d", &val);
            printf("%2d ", val);
        }
        printf("\n");
    }
    
    fclose(fp[1]);
    
    return 0;
}
