#ifndef SIMPLE_STATUS_H
#define SIMPLE_STATUS_H

#ifndef __linux__
#error "This works on Linux only"
#endif

#define _DEFAULT_SOURCE

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdnoreturn.h>
#include <stdarg.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <unistd.h>

#ifndef __GNUC__
#define __attribute__(x)
#endif

#define size(a) sizeof(a)/sizeof(*a)

struct colors {
        unsigned long fg, bg;
};

extern struct colors colors_normal, colors_highlight;

extern void error(const char *format, ...)
        __attribute__((format(printf, 1, 2)));

extern int pscanf(const char *path, const char *format, ...)
        __attribute__((format(scanf, 2, 3)));

#endif
