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

/*
 *  ANSI esc sequences for colours + formatting for stdout
 */
#define BOLD(text)              "\033[1m"text"\033[0m"
#define ITALIC(text)            "\033[3m"text"\033[0m"

#define BLACK(text)             "\033[30m"text"\033[0m"
#define RED(text)               "\033[31m"text"\033[0m"
#define GREEN(text)             "\033[32m"text"\033[0m"
#define YELLOW(text)            "\033[33m"text"\033[0m"
#define BLUE(text)              "\033[34m"text"\033[0m"
#define MAGENTA(text)           "\033[35m"text"\033[0m"
#define CYAN(text)              "\033[36m"text"\033[0m"
#define WHITE(text)             "\033[37m"text"\033[0m"
#define BRIGHT_BLACK(text)      "\033[90m"text"\033[0m"
#define BRIGHT_RED(text)        "\033[91m"text"\033[0m"
#define BRIGHT_GREEN(text)      "\033[92m"text"\033[0m"
#define BRIGHT_YELLOW(text)     "\033[93m"text"\033[0m"
#define BRIGHT_BLUE(text)       "\033[94m"text"\033[0m"
#define BRIGHT_MAGENTA(text)    "\033[95m"text"\033[0m"
#define BRIGHT_CYAN(text)       "\033[96m"text"\033[0m"
#define BRIGHT_WHITE(text)      "\033[97m"text"\033[0m"

#endif
