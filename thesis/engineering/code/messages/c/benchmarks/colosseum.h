#pragma once

#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>

typedef enum {
	T_U8   = 0x00, T_U16 = 0x01, T_U32  = 0x02, T_U64  = 0x03,
	T_I8   = 0x04, T_I16 = 0x05, T_I32  = 0x06, T_I64  = 0x07,
	T_F32  = 0x08, T_F64 = 0x09, T_CHAR = 0x0A, T_BOOL = 0x0B,
} Arg_Type;

void mscanf(u8 const *buf, char const *fmt, ...);

u32 msavef(i8 tag, u8 *buf, char const *fmt, ...);

i32 msendf(int f, i8 tag, char const *fmt, ...);

i32 msend(int f, i8 tag, void const *buf, u32 size);

i32 mrecv(int f, i8 *tag, void *buf, u32 size);

i32 mping(int f, i8 tag);
