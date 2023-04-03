#define _POSIX_C_SOURCE 199309L
#define DEBUG
#include <stdio.h>
#include <assert.h>

// only included for message tags, the player doesn't have to simulate the game
#include "pentago.c"

#include "colosseum.c"

i32 score_board(char *board, i32 size, char player) {
    i32 result = 0;

    for (i32 i = 0; i < size; i++) {
        char color = 0;
        i32 in_a_row = 0;
        for (i32 j = 0; j < size; j++) {
            char f = board[size*i + j];
            if (f == color) {
                in_a_row++;
                i32 mul = (player == color) ? 1 : -(color != '.');
                if (in_a_row >= size/2 + 2)
                    result += 1000 * mul;
                else
                    result += in_a_row * in_a_row * mul;
            } else {
                in_a_row = 1;
                color = f;
            }
        }
    }
    for (i32 i = 0; i < size; i++) {
        char color = 0;
        i32 in_a_row = 0;
        for (i32 j = 0; j < size; j++) {
            char f = board[size*j + i];
            if (f == color) {
                in_a_row++;
                i32 mul = (player == color) ? 1 : -(color != '.');
                if (in_a_row >= size/2 + 2)
                    result += 1000 * mul;
                else
                    result += in_a_row * in_a_row * mul;
            } else {
                in_a_row = 1;
                color = f;
            }
        }
    }
    return result;
}

i32 get_and_score_board(int in, int out, char player) {
    mping(out, MSG_GET_BOARD);
    u8 buf[256*256+64];
    i8 tag;
    i32 res = mrecv(in, &tag, buf, sizeof(buf));
    assert(tag == MSG_GET_BOARD);
    i32 height, width;
    char board[256*256];
    mscanf(buf, "%u[%<=256,%<=256]", board, &height, &width);
    assert(height == width);
    return score_board(board, width, player);
}

typedef struct {
    i32 score;
    PentagoMove move;
} PickResult;

PickResult pick_best_move(int in, int out, char player, int max_depth, i32 alpha, i32 betha) {
    assert(max_depth >= 0);
    u8 buf[0x1000];
    i32 i_count, j_count, r_count;
    u8 i[1024];
    u8 j[1024];
    u8 r[1024];

    i8 tag;
    i32 res;

    res = mping(out, MSG_GET_MOVES);
    res = mrecv(in, &tag, buf, sizeof(buf));
    assert(tag == MSG_GET_MOVES);
    mscanf(buf, "%u[%<=1024] %u[%<=1024] %u[%<=1024]",
            i, &i_count,
            j, &j_count,
            r, &r_count);
    assert(i_count == j_count && j_count == r_count);
    //printf("max_depth %d got %d moves\n", max_depth, i_count);

    PickResult result;
    result.move = (PentagoMove){i[0], j[0], r[0]};
    result.score = -1000000;
    for (i32 move = 0; move < i_count; move++) {
        //printf("testing move (%u, %u) %u\n", i[move], j[move], r[move]);
        msendf(out, MSG_MAKE_MOVE, "%u %u %u", i[move], j[move], r[move]);

        // check if the current move wins
        res = mping(out, MSG_GET_WINNER);
        res = mrecv(in, &tag, buf, sizeof(buf));
        assert(tag == MSG_GET_WINNER);
        char winner;
        mscanf(buf, "%u", &winner);
        if (winner) {
            mping(out, MSG_UNDO_MOVE);
            if (winner == player) {
                result.score = 1000;
                result.move = (PentagoMove){i[move], j[move], r[move]};
                return result;
            } else {
                if (winner == 'D' && result.score < 0) {
                    result.score = 0;
                    result.move = (PentagoMove){i[move], j[move], r[move]};
                }
                continue;
            }
        }

        i32 score;
        if (max_depth == 0) {
            score = get_and_score_board(in, out, player);
        } else {
            char other_p = (player == 'B') ? 'W' : 'B';
            PickResult pick = pick_best_move(in, out, other_p, max_depth-1, -betha, -alpha);
            score = -pick.score;
        }

        if (score > result.score) {
            result.score = score;
            result.move = (PentagoMove){i[move], j[move], r[move]};
            alpha = score;
        }

        mping(out, MSG_UNDO_MOVE);

        if (alpha >= betha) return result;
    }
    return result;
}

int main(int argc, char **argv) {
    srand(time(0));
    int in  = open(argv[1], O_RDONLY);
    int out = open(argv[2], O_WRONLY);

    u8 buf[0x1000];

    mping(out, MSG_GET_PLAYER);
    char player;
    i8 tag;
    mrecv(in, &tag, buf, sizeof(buf));
    assert(tag == MSG_GET_PLAYER);
    mscanf(buf, "%u", &player);
    printf("got player %c\n", player);

    i32 i_count, j_count, r_count;
    u8 i[1024];
    u8 j[1024];
    u8 r[1024];

    while (1) {
        PickResult pick = pick_best_move(in, out, player, 3, INT32_MIN, INT32_MAX);
        PentagoMove m = pick.move;
        printf("%c picked (%u, %u) %u with score %d\n", player, m.i, m.j, m.rotation, pick.score);
        msendf(out, MSG_COMMIT_MOVE, "%u %u %u", m.i, m.j, m.rotation);
    }
}
