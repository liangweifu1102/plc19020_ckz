#ifndef _TYPEDEFINE_H_
#define _TYPEDEFINE_H_

#include <stdint.h>
#include <stdbool.h>

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t uint8;

typedef int32_t int32;
typedef int16_t int16;
typedef int8_t int8;

typedef volatile uint32_t vu32;
typedef volatile uint16_t vu16;
typedef volatile uint8_t vu8;

typedef union {
    unsigned char Val;
    struct
    {
        unsigned char b0 : 1;
        unsigned char b1 : 1;
        unsigned char b2 : 1;
        unsigned char b3 : 1;
        unsigned char b4 : 1;
        unsigned char b5 : 1;
        unsigned char b6 : 1;
        unsigned char b7 : 1;
    } bits;

} BYTE_BITS;

typedef union {
    unsigned char Val;
    struct
    {
        unsigned char b10 : 2;
        unsigned char b32 : 2;
        unsigned char b54 : 2;
        unsigned char b76 : 2;
    } twobits;

} BYTE_TWOBITS;

typedef union {
    unsigned short Val;
    unsigned char v[2];
    struct
    {
        unsigned char LB;
        unsigned char HB;
    } byte;
    struct
    {
        unsigned char b0 : 1;
        unsigned char b1 : 1;
        unsigned char b2 : 1;
        unsigned char b3 : 1;
        unsigned char b4 : 1;
        unsigned char b5 : 1;
        unsigned char b6 : 1;
        unsigned char b7 : 1;
        unsigned char b8 : 1;
        unsigned char b9 : 1;
        unsigned char b10 : 1;
        unsigned char b11 : 1;
        unsigned char b12 : 1;
        unsigned char b13 : 1;
        unsigned char b14 : 1;
        unsigned char b15 : 1;
    } bits;
} WORD_BITS;

typedef union {
    unsigned long V32;
    unsigned short V16[2];
    struct
    {
        unsigned short V16L;
        unsigned short V16H;
    } word;
    unsigned char V8[4];
    struct
    {
        unsigned char V8LLLH;
        unsigned char V8LLHL;
        unsigned char V8LHLL;
        unsigned char V8HLLL;
    } byte;
    struct
    {
        unsigned char b0 : 1;
        unsigned char b1 : 1;
        unsigned char b2 : 1;
        unsigned char b3 : 1;
        unsigned char b4 : 1;
        unsigned char b5 : 1;
        unsigned char b6 : 1;
        unsigned char b7 : 1;
        unsigned char b8 : 1;
        unsigned char b9 : 1;
        unsigned char b10 : 1;
        unsigned char b11 : 1;
        unsigned char b12 : 1;
        unsigned char b13 : 1;
        unsigned char b14 : 1;
        unsigned char b15 : 1;
        unsigned char b16 : 1;
        unsigned char b17 : 1;
        unsigned char b18 : 1;
        unsigned char b19 : 1;
        unsigned char b20 : 1;
        unsigned char b21 : 1;
        unsigned char b22 : 1;
        unsigned char b23 : 1;
        unsigned char b24 : 1;
        unsigned char b25 : 1;
        unsigned char b26 : 1;
        unsigned char b27 : 1;
        unsigned char b28 : 1;
        unsigned char b29 : 1;
        unsigned char b30 : 1;
        unsigned char b31 : 1;
    } bits;
} DWORD_VAL;

#endif
