#ifndef SIMPLE_STATUS_H
#define SIMPLE_STATUS_H

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

#ifndef __GNUC__
#define __attribute__(x)
#endif

#define size(a) sizeof(a)/sizeof(*a)

extern void error(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

#endif
