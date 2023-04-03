#include "colosseum.h"
#include "stdio.h" 
#include "stdlib.h" 
#include "assert.h" 

int main(int argc, char **argv) {
	u8 board[248][6][6] = {0};
	u32 d1, d2, d3;
	u8  a = 42;
	u32 b = 1024;
	i8  c = 42;
	i32 d = 1024;
	f32 e = 3.1415;
	f64 f = 3.1415;
	for (int i = 0; i < 248; i++)
		for (int j = 0; j < 6; j++)
			for (int k = 0; k < 6; k++)
				board[i][j][k] = i*j;
	u8 buf[0x4000];
	u32 buf_len;

	u64 total_encode_matrix = 0;
	for (int i = 0; i < 1e6; i++) {
		timespec t0 = gettime();
		buf_len = msavef(0, buf, "%u[248,6,6]", board);
		timespec t1 = gettime();
		total_encode_matrix += deltatime(t0, t1);
	}

	u64 total_decode_matrix = 0;
	for (int i = 0; i < 1e6; i++) {
		timespec t0 = gettime();
		mscanf(buf + 9, "%u[%,%,%]", board, &d1, &d2, &d3);
		timespec t1 = gettime();
		total_decode_matrix += deltatime(t0, t1);
	}

	u64 total_encode_simple = 0;
	for (int i = 0; i < 1e6; i++) {
		timespec t0 = gettime();
		buf_len = msavef(0, buf, "%u %U %i %I %f %F", a, b, c, d, e, f);
		timespec t1 = gettime();
		total_encode_simple += deltatime(t0, t1);
	}

	u64 total_decode_simple = 0;
	for (int i = 0; i < 1e6; i++) {
		timespec t0 = gettime();
		mscanf(buf + 9, "%u %U %i %I %f %F", &a, &b, &c, &d, &e, &f);
		timespec t1 = gettime();
		total_decode_simple += deltatime(t0, t1);
	}

	printf("encode simple %.6f\n", total_encode_simple * 1e-9);
	printf("decode simple %.6f\n", total_decode_simple * 1e-9);
	printf("encode matrix %.6f\n", total_encode_matrix * 1e-9);
	printf("decode matrix %.6f\n", total_decode_matrix * 1e-9);
	return 0;
}
