/**
 * @file log.c
 * @author Zhang Hai
 */

#include "log.h"

#include <stdio.h>
#include <stdarg.h>

#include "common.h"

// log() is inside math
static void log_log(char *msg, char *format, va_list args);

void log_info(char *format, ...) {
    va_list args;
    va_start(args, format);
    log_log("INFO", format, args);
    va_end(args);
}

void log_warn(char *format, ...) {
    va_list args;
    va_start(args, format);
    log_log("WARN", format, args);
    va_end(args);
}

void log_error(char *format, ...) {
    va_list args;
    va_start(args, format);
    log_log("ERROR", format, args);
    va_end(args);
}

static void log_log(char *msg, char *format, va_list args) {
    fprintf(stderr, "[%s] ", msg);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
}
