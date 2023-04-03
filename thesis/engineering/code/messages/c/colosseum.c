#include "colosseum.h"
#include <alloca.h>

u8 const max_fields = 8;
u8 const max_dim = 8;
u32 const max_types = (4*max_dim+1)*max_fields;

Arg_Type const format_type[] = {
	['u']=T_U8 , ['U']=T_U32, ['i']=T_I8, ['I']=T_I32, ['f']=T_F32, ['F']=T_F64, ['c']=T_CHAR, ['b']=T_BOOL
};

u32 const format_size[] = {
	['u']=1, ['U']=4, ['i']=1, ['I']=4, ['f']=4, ['F']=8, ['c']=1, ['b']=1
};

u32 const type_size[] = {
	[T_U8]=1, [T_U32]=4, [T_I8]=1, [T_I32]=4, [T_F32]=4, [T_F64]=8, [T_CHAR]=1, [T_BOOL]=1,
};

u32 parse_num(char const **fmt) {
	u32 res = 0;
	while ('0' <= **fmt && **fmt <= '9') {
		res = res * 10 + **fmt - '0';
		*fmt += 1;
	}
	return res;
}

void mscanf(u8 const *buf, char const *fmt, ...) {
	u32 field = 0, type = 0, data = 0;
	u32 sizes[max_fields];
	void *fields[max_fields];

	va_list args;
	va_start(args, fmt);
	while (*fmt) {
		if (*fmt++ != '%') continue;
		if (field >= max_fields) panic("message must have at most %d fields, but found %d\n", max_fields, field);
		u8 msg_type = buf[type++];
		u8 msg_base = msg_type >> 0 & 0x0F;
		u8 msg_dims = msg_type >> 4 & 0x07;
		u8 exp_type = format_type[(u8) *fmt++];
		sizes[field] = type_size[msg_base];
		fields[field] = va_arg(args, void*);
		if (exp_type != msg_base)
			panic("message field %d expected base type %02X, but got %02X\n", field, exp_type, msg_base);
		if (*fmt == '[') {
			u8 fmt_dims = 0;
			fmt++;
			while (*fmt != ']') {
				u32 dim = 0, max = 0;
				fmt_dims++;
				if (*fmt == '%') {
					fmt++;
					dim = max = *((u32*)(buf + type));
					type += 4;
					if (fmt[0] == '<' && fmt[1] == '=') {
						fmt += 2;
						max = parse_num(&fmt);
					}
					*va_arg(args, u32*) = dim;
				} else {
					dim = max = parse_num(&fmt);
				}
				sizes[field] *= dim;
				if (dim > max) panic("array dimimension %d must be less than %d, as specified in the format, but got %d\n", fmt_dims, max, dim);
				//if (dim == 0 || max == 0) panic("array dimensions must be greater than 0, but got %d\n", dim);
				if (*fmt == ']') break;
				if (*fmt != ',') panic("expected `,` but got `%c`\n", *fmt);
                fmt++;
			}
			fmt++;
			if (fmt_dims != msg_dims) panic("array dimensions don't match (%d != %d)\n", fmt_dims, msg_dims);
			if (fmt_dims == 0) panic("array must have at least one dimension\n");
			if (fmt_dims > max_dim) panic("array must have at most %u dimensions, but got %u\n", max_dim, fmt_dims);
		}
		data += sizes[field];
		field++;
	}
	va_end(args);

	for (u32 i = 0, offset = 0; i < field; i++) {
		memcpy(fields[i], buf + type + offset, sizes[i]);
		offset += sizes[i];
	}
}

i32 msendf(int f, i8 tag, char const *fmt, ...) {
	void *fields[max_fields];
	u32 sizes[max_fields];
	u8 types[max_types];
	u32 field = 0, type = 0, data = 0;

	va_list args;
	va_start(args, fmt);
	while (*fmt) {
		if (*fmt++ != '%') continue;
		if (field >= max_fields) panic("message must have at most %d fields (found %d)\n", max_fields, field);
		u8 base = format_type[(u8) *fmt++];
		sizes[field] = type_size[base];
		void *arg = va_arg(args, void*);
		if (*fmt == '[') {
			u32 dims[8];
			u8 n_dims = 0;
			fmt++;
			while (*fmt != ']') {
				u32 dim;
				if (*fmt == '%') {
					fmt++;
					dim = va_arg(args, u32);
				} else {
					dim = parse_num(&fmt);
				}
				dims[n_dims++] = dim;
				//if (dim == 0) panic("array dimensions must be greater than 0\n");
				if (*fmt == ']') break;
				if (*fmt != ',') panic("expected `,` or `]` but got `%c`\n", *fmt);
				fmt++;
			}
			fmt++;
			if (n_dims == 0) panic("array must have at least one dimension\n");
			if (n_dims > max_dim) panic("array must have at most %u dimensions, but got %u\n", max_dim, n_dims);
			types[type++] = (0x07 & n_dims) << 4 | (base & 0x0F);
			for (int i = 0; i < n_dims; i++) {
				u32 dim = dims[i];
				sizes[field] *= dim;
				*((u32*)(types + type)) = dim;
				type += 4;
			}
			fields[field] = arg;
		} else {
			types[type++] = base & 0x0F;
			fields[field] = alloca(sizes[field]);
			memcpy(fields[field], &arg, sizes[field]);
		}
		data += sizes[field];
		field++;
	}
	va_end(args);

	u32 buf_len = 9 + type + data;
	u8 buf[buf_len];
	memcpy(buf, &tag, 1);
	memcpy(buf+1, &type, 4);
	memcpy(buf+5, &data, 4);
	memcpy(buf+9, &types, type);
	for (u32 i = 0, offset = 9 + type; i < field; i++) {
		memcpy(buf + offset, fields[i], sizes[i]);
		offset += sizes[i];
	}

	//hexdump(buf+9, buf_len-9); printf("\n");
	return write(f, buf, buf_len);
}

i32 msend(int f, i8 tag, void const *buf, u32 size) {
    i32 sent = write(f, &tag, 1);
	if (sent < 1) return sent;
	return write(f, buf, size);
}

i32 mrecv(int f, i8 *tag, void *buf, u32 max_size) {
	u8 head[9];
	u32 head_size = read(f, head, 9);
	if (head_size != 9)
		panic("expected header of size 9, but received %u bytes\n", head_size);

	*tag = *((i8*)(head));
	u32 type_size = *((u32*)(head + 1));
	u32 data_size = *((u32*)(head + 5));
	u32 exp_size = type_size + data_size;
	if (exp_size > max_size)
		panic("incoming message size %u exceeded maximum buffer size %u\n", exp_size, max_size);

	u32 pay_size = read(f, buf, exp_size);
	if (exp_size != pay_size)
		panic("expected payload of size %u, but received %u bytes\n", exp_size, pay_size);

	//printf("recv "); hexdump(tag, 1); hexdump(&type_size, 4); hexdump(&data_size, 4);
	//hexdump(buf, pay_size); printf("\n");

	return pay_size;
}

i32 mping(int f, i8 tag) {
	u8 head[9] = {0};
    head[0] = (u8)tag;
    return write(f, head, 9);
}
