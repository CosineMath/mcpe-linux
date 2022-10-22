
#include "log.h"
#include <stdio.h>
#include <stdarg.h>

int __android_log_write(int prio, const char* tag, const char* text) {
    return fprintf(stderr, "[Android/Level=%d] [%s] %s\n", prio, tag, text);
}

int __android_log_print(int prio, const char* tag, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "[Android/Level=%d] [%s] ", prio, tag);
    int ret = vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    return ret;
}

