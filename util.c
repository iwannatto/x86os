#include "util.h"

#include "graphic.h"
#include <stdarg.h>

int vsprintf(char *restrict s, const char *restrict format, va_list ap)
{
    int n = 0;
    while (*format != '\0') {
        if (*format == '%' && (*(format + 1) == 'd')) {
            format += 2;

            int i = va_arg(ap, int);
            if (i == 0) {
                *s = '0';
                ++s;
                ++n;
                continue;
            }

            /* don't think about INT_MIN */
            if (i < 0) {
                *s = '-';
                ++s;
                ++n;

                i = -i;
            }

            int pow10 = 1;
            while ((i / (pow10 * 10)) > 0) {
                pow10 *= 10;
            }

            while (pow10 > 0) {
                *s = '0' + (i / pow10);
                ++s;
                ++n;
                i -= (i / pow10) * pow10;
                pow10 /= 10;
            }
        } else {
            *s = *format;
            ++s;
            ++format;
            ++n;
        }
    }
    *s = '\0';
    ++n;
    return n;
}

int sprintf(char *restrict s, const char *restrict format, ...)
{
    va_list ap;
    va_start(ap, format);
    int n = vsprintf(s, format, ap);
    va_end(ap);
    return n;
}

/* font size and vram size are hard-coded */
int printf(const char *restrict format, ...)
{
    static int x = 0;
    static int y = 0;

    char s[100];
    va_list ap;
    va_start(ap, format);
    int n = vsprintf(s, format, ap);
    va_end(ap);

    for (int i = 0; s[i] != '\0'; ++i) {
        if (s[i] == '\n') {
            x = 0;
            y += 16;
            continue;
        }

        drawchar(x, y, WHITE, s[i]);
        x += 8;
        if (x >= 320 - 7) {
            x = 0;
            y += 16;
        }
    }

    return n;
}
