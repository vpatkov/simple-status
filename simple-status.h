#ifndef SIMPLE_STATUS_H
#define SIMPLE_STATUS_H

#define _POSIX_C_SOURCE 199309L

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

extern void error(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

#endif
