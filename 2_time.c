#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

//extern char *tzname[];

int main() {
    time_t now;  //календарное время, тип данных, эквивалентент long int
    struct tm *sp;  //местное время, тип данных

   /* if(putenv("TZ=California+7") != 0) { //заменяет существующую переменную окружения, выдает ошибку при (-1)
        perror("Error");
        return 0;
    }*/

    (void) time(&now); //переменная now типа time_t, время сохраняется в переменной now

    printf("%s", ctime(&now)); //ctime преобразует календарное время в ASCII-строку формата даты, адрес используется в качестве параметра printf

    sp = localtime(&now); //заполняет значениями поля структуры
    if (!sp) {
        perror("Error");
        return 0;
    }

    printf("%d/%d/%02d %d:%02d %s\n",
           sp->tm_mon + 1, sp->tm_mday,
           sp->tm_year + 1900, sp->tm_hour,
           sp->tm_min, tzname[sp->tm_isdst]);
    exit(0);
}
