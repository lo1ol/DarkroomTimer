#include "Arduino.h"

#include <assert.h>
#include <string.h>
#include <unistd.h>

/* reverse:  переворачиваем строку s на месте */
void reverse(char s[]) {
    int i, j;
    char c;

    for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

void itoa(int n, char s[], int base) {
    assert(base == 10);
    int i, sign;

    if ((sign = n) < 0)
        n = -n;
    i = 0;
    do {                       /* генерируем цифры в обратном порядке */
        s[i++] = n % 10 + '0'; /* берем следующую цифру */
    } while ((n /= 10) > 0);   /* удаляем */
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
}

void delay(uint32_t) {
    // TODO
}

void delayMicroseconds(uint32_t) {
    // TODO
}
