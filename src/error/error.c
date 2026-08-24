#include "error/error.h"
#include <stdarg.h>
#include <stdio.h>

void error_report(const char *fmt, ...)
{
    fprintf(stderr, "error: ");
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}

void warning_report(const char *fmt, ...)
{
    fprintf(stderr, "warning: ");
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}

void parser_error_report(int line, int col, const char *fmt, ...)
{
    fprintf(stderr, "line %d, col %d: error: ", line, col);
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}

void parser_warning_report(int line, int col, const char *fmt, ...)
{
    fprintf(stderr, "line %d, col %d: warning: ", line, col);
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}