#pragma once

#define _POSIX_C_SOURCE 199309L
#include <stdint.h>
#include <assert.h>
#include <time.h>

#define panic(fmt, args...) do { fprintf(stderr, "[error %s -> %s] " fmt, __FILE__, __func__, ##args); exit(1); } while (0)

#ifdef DEBUG
#define debug(fmt, args...) do { fprintf(stderr, fmt, ##args); } while (0)
#else
#define debug(fmt, args...) do {} while(0)
#endif

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef float    f32;
typedef double   f64;

typedef struct timespec timespec;

i64 deltatime(timespec t0, timespec t1);

timespec gettime();

void hexdump(void const *buffer, u32 size);
