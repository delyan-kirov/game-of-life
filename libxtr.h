#ifndef LIBXTR_H

#include "stdio.h"
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------------------------|
 *                              LIBRARY DEFINITIONS                              |
 * ------------------------------------------------------------------------------|
 */

#define MINI_STR_MAX_LEN ((1 << 8) - 1)
#define STR_DEFAULT_LEN (32 * sizeof(MiniStr))

typedef struct
{
    char *buf;
    size_t len;
    size_t capacity;
} String;

typedef struct
{
    unsigned char len;
    char buf[MINI_STR_MAX_LEN];
} MiniStr;

// [size..][size..]...[size...]^******
typedef struct StrBld
{
    char *buf;
    size_t len;
    size_t capacity;
} StrBld;

/*
 * Create a new string builder object by passing a pointer
 */
void StrBld_new(StrBld *strBld);
/*
 * Reset string builder len paramenter
 * Useful for reusing a string builder without allocating new memory
 */
void StrBld_reset(StrBld *strBld);
/*
 * Free all memory allocated by string builder
 * Intended to be used together with StrBld_new
 */
void StrBld_free(StrBld *strBld);
/*
 * Add a string to the string builder object
 * NOTE: this function clones all strings passed
 */
void StrBld_add(StrBld *strBld, char *str);
/*
 * Add many strings to string builder object at once
 * NOTE: must end with empty string ie: '\0'
 * NOTE: this is a variadic function
 * NOTE: this does a clone of all strings passed
 */
void StrBld_add_many(StrBld *strBld, ...);
/*
 * Generate the final string from the string builder
 * NOTE: should not reuse the string builder, unless it is reset with StrBld_reset
 */
String StrBld_fuse(StrBld *strBld);
/*
 * Generate the final string from the string builder using a format.
 * Example: passing "%s_%s", s1, s2: will apply the prefix s1 and suffix s2 to all strings in string builder.
 * Example: passong "_%s_", s1: will apply the infix s1 to all strings in string builder.
 * NOTE: should not reuse the string builder, unless it is reset with StrBld_reset.
 */
String StrBld_fuse_formated(StrBld *strBld, char *format...);

/* ------------------------------------------------------------------------------|
 *                                IMPLEMENTATION                                 |
 * ------------------------------------------------------------------------------|
 */

void StrBld_new(StrBld *strBld)
{
    strBld->len = 0;
    strBld->capacity = STR_DEFAULT_LEN;
    strBld->buf = malloc(sizeof(MiniStr *) * (strBld->capacity));
}

void StrBld_reset(StrBld *strBld)
{
    strBld->len = 0;
}

void StrBld_free(StrBld *strBld)
{
    free(strBld->buf);
    strBld->capacity = 0;
    strBld->len = 0;
    strBld = NULL;
}

void StrBld_add(StrBld *strBld, char *str)
{
    if (str == NULL) return;

    size_t str_len = strlen(str);
    strBld->buf[strBld->len] = str_len;
    memcpy(strBld->buf + strBld->len + 1, str, str_len);

    strBld->len += sizeof(MiniStr);
}

void StrBld_add_many(StrBld *strBld, ...)
{
    va_list strs;
    char *str = NULL;

    va_start(strs, strBld);

    while (NULL != (str = va_arg(strs, char *)))
        StrBld_add(strBld, str);

    va_end(strs);
}

String StrBld_fuse(StrBld *strBld)
{
    size_t strLen = 0;
    String outStr = {0};

    // Calculate total length of all strings
    for (size_t it = 0; it < strBld->len; it += sizeof(MiniStr))
        strLen += strBld->buf[it];

    outStr.capacity = strLen;
    outStr.len = 0;
    outStr.buf = malloc(sizeof(char) * (strLen + 1)); // Allocate space for the string and null terminator

    // Concatenate strings
    for (size_t it = 0; it < strBld->len; it += sizeof(MiniStr))
    {
        memcpy(outStr.buf + outStr.len, strBld->buf + it + 1, strBld->buf[it]);
        outStr.len += strBld->buf[it];
    }

    outStr.buf[outStr.len] = '\0'; // Null-terminate the result string

    return outStr;
}

#endif //  LIBXTR_H
