#define _POSIX_C_SOURCE 199309L
#define DEBUG
#include <stdio.h>
#include <assert.h>

#include "colosseum.c"

typedef enum {
    MSG_NONE        = 0,
    MSG_MAKE_MOVE   = 1,
    MSG_COMMIT_MOVE = 2,
    MSG_GET_MOVES   = 3,
    MSG_UNDO_MOVE   = 4,
    MSG_GET_WINNER  = 5,
    MSG_GET_BOARD   = 6,
    MSG_GET_PLAYER  = 7,
} PentagoMsg;

int main(int argc, char **argv) {
    srand(time(0));
    int in  = open(argv[1], O_RDONLY);
    int out = open(argv[2], O_WRONLY);

    u8 buf[0x1000];
    i32 i_count, j_count, r_count;
    u8 i[1024];
    u8 j[1024];
    u8 r[1024];

    while (1) {
        i32 res = mping(out, MSG_GET_MOVES);
        i8 tag;
        res = mrecv(in, &tag, buf, 0x1000);
        assert(tag == MSG_GET_MOVES);
        mscanf(buf, "%u[%<=1024] %u[%<=1024] %u[%<=1024]",
                i, &i_count,
                j, &j_count,
                r, &r_count);
        assert(i_count == j_count && j_count == r_count);

        u32 move = rand() % i_count;
        res = msendf(out, MSG_COMMIT_MOVE, "%u %u %u", i[move], j[move], r[move]);
    }
}
