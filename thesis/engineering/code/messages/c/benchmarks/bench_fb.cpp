#include "utils.h"
#include "stdio.h" 
#include "stdlib.h" 
#include "assert.h" 
#include "flexbuffers.h"

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
	std::vector<u8> buf;

	u64 total_encode_simple = 0;
	for (int i = 0; i < 1e6; i++) {
		timespec t0 = gettime();
		flexbuffers::Builder fbb;
		fbb.Vector([&]() {
			fbb.UInt(a);
			fbb.UInt(b);
			fbb.Int(c);
			fbb.Int(d);
			fbb.Double(e);
			fbb.Double(f);
		});
		fbb.Finish();
		buf = fbb.GetBuffer();
		timespec t1 = gettime();
		total_encode_simple += deltatime(t0, t1);
	}

	u8  ap;
	u32 bp;
	i8  cp;
	i32 dp;
	f64 ep;
	f64 fp;
	u64 total_decode_simple = 0;
	for (int i = 0; i < 1e6; i++) {
		timespec t0 = gettime();
		auto vec = flexbuffers::GetRoot(buf).AsVector();
		memcpy(&ap, vec[0].data_, 1);
		memcpy(&bp, vec[1].data_, 4);
		memcpy(&cp, vec[2].data_, 1);
		memcpy(&dp, vec[3].data_, 4);
		memcpy(&ep, vec[4].data_, 8);
		memcpy(&fp, vec[5].data_, 8);
		timespec t1 = gettime();
		total_decode_simple += deltatime(t0, t1);
	}

	u64 total_encode_matrix = 0;
	for (int i = 0; i < 1e6; i++) {
		timespec t0 = gettime();
		flexbuffers::Builder fbb;
		fbb.Vector([&]() {
			fbb.UInt(248);
			fbb.UInt(6);
			fbb.UInt(6);
			fbb.Blob(board, 248*6*6);
		});
		timespec t1 = gettime();
		total_encode_matrix += deltatime(t0, t1);
	}

	u8 blob[248][6][6];
	u64 total_decode_matrix = 0;
	for (int i = 0; i < 1e6; i++) {
		timespec t0 = gettime();
		auto vec = flexbuffers::GetRoot(buf).AsVector();
		memcpy(blob, vec[3].AsBlob().data(), 248*6*6);
		timespec t1 = gettime();
		total_decode_matrix += deltatime(t0, t1);
	}

	printf("%u\n", blob[100]);
	printf("%u %u %i %i %f %f\n", ap, bp, cp, dp, ep, fp);

	printf("fb encode simple %.6f\n", total_encode_simple * 1e-9);
	printf("fb decode simple %.6f\n", total_decode_simple * 1e-9);
	printf("fb encode matrix %.6f\n", total_encode_matrix * 1e-9);
	printf("fb decode matrix %.6f\n", total_decode_matrix * 1e-9);
	return 0;
}
