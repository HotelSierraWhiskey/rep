#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>

#define ASSERT(x, ...) \
    if (!(x)) { \
        fprintf(stderr, "Assertion failed: %s\nFile: %s, function: %s, line: %d\n", \
                #x, __FILE__, __func__, __LINE__); \
        if (*#__VA_ARGS__) { \
            fprintf(stderr, "Message: " __VA_ARGS__); \
            fprintf(stderr, "\n"); \
        } \
        abort(); \
    }

#define FALLTHROUGH     __attribute__ ((fallthrough));

#define UNUSED(x)       (void)(x)

#define GREEN(text)     "\033[32m"text"\033[0m"

#endif
