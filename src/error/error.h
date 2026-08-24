#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>

/* General engine/CLI error – no line/col */
void error_report(const char *fmt, ...);

/* General engine/CLI warning – no line/col */
void warning_report(const char *fmt, ...);

/* Parser source‑located error (line, col) */
void parser_error_report(int line, int col, const char *fmt, ...);

/* Parser source‑located warning (line, col) */
void parser_warning_report(int line, int col, const char *fmt, ...);

#endif