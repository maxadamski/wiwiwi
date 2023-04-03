#define _POSIX_C_SOURCE 199309L
#define DEBUG
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "pentago.c"
#include "colosseum.c"

typedef struct timespec timespec;

timespec get_time() {
    timespec temp;
    //clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &temp);
    clock_gettime(CLOCK_MONOTONIC, &temp);
    return temp;
}

timespec time_diff(timespec start, timespec end) {
    timespec temp;
    if (end.tv_nsec < start.tv_nsec) {
        temp.tv_sec  = end.tv_sec - start.tv_sec - 1;
        temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
    } else {
        temp.tv_sec  = end.tv_sec - start.tv_sec;
        temp.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    return temp;
}

bool time_less_than(timespec a, timespec b) {
    timespec res = time_diff(a, b);
    return res.tv_sec < 0 || res.tv_nsec < 0;
}

timespec time_from_ms(i32 ms) {
    timespec result = {
        .tv_sec  = ms / 1000000,
        .tv_nsec = 1000 * (ms % 1000000)
    };
    return result;
}

typedef struct {
    int in;
    int out;
    Pentago game;
    PentagoMove *stack;
} Player;

typedef enum {
    PLAYER_OK      = 0,
    PLAYER_ILLEGAL = 1,
    PLAYER_TIMEOUT = 2,
} HandlePlayerResult;

HandlePlayerResult handle_player(Player *p, Pentago *game, i32 timeout) {
    int in  = p->in;
    int out = p->out;

    // prepare player's local copy of the game
    i32 size = game->board_size * game->board_size;
    memcpy(p->game.board, game->board, size);
    p->game.current_player = game->current_player;
    p->game.winner = game->winner;
    p->game.board_size = game->board_size;
    arrsetlen(p->stack, 0);

    timespec time_move_max = time_from_ms(timeout);
    timespec time_move_start = get_time();

    bool done = false;
    while (!done) {
        i8 tag;
        u8 buffer[0x1000] = {};
        i32 received = mrecv(out, &tag, buffer, 0x1000);

        switch (tag) {
            case MSG_COMMIT_MOVE: {
                u8 i, j, rotation;
                mscanf(buffer, "%u %u %u", &i, &j, &rotation);
                eprintf("Player %c: (%u, %u) %u\n", game->current_player, i, j, rotation);

                timespec time_move_end = get_time();
                timespec time_move = time_diff(time_move_start, time_move_end);
                eprintf("Took %d.%09d seconds\n", time_move.tv_sec, time_move.tv_nsec);

                PentagoError err = make_move(game, i, j, rotation);
                if (err) {
                    eprintf("MSG_COMMIT_MOVE error %d\n", err);
                    return PLAYER_ILLEGAL;
                }
                done = true;
            } break;

            case MSG_GET_MOVES: {
                PentagoMoves moves;
                moves = get_available_moves(&p->game);
                msendf(in, MSG_GET_MOVES, "%u[%] %u[%] %u[%]",
                        moves.i, moves.count,
                        moves.j, moves.count,
                        moves.rotation, moves.count);
                free_moves(&moves);
            } break;

            case MSG_MAKE_MOVE: {
                u8 i, j, rotation;
                mscanf(buffer, "%u %u %u", &i, &j, &rotation);
                int no_rotation = 0;
                make_move_(&p->game, i, j, rotation, &no_rotation);
                if (no_rotation) rotation = 8;
                PentagoMove move = {i, j, rotation};
                arrpush(p->stack, move);
            } break;

            case MSG_UNDO_MOVE: {
                if (arrlen(p->stack) == 0) {
                    eprintf("MSG_UNDO_MOVE error: no move to undo");
                    return PLAYER_ILLEGAL;
                }
                PentagoMove move = arrpop(p->stack);
                PentagoError err;
                err = undo_move(&p->game, move.i, move.j, move.rotation);
                if (err) {
                    eprintf("undoing move (%u, %u) %u\n", move.i, move.j, move.rotation);
                    eprintf("ERR %d\n", err);
                    return PLAYER_ILLEGAL;
                }
            } break;

            case MSG_GET_WINNER: {
                msendf(in, MSG_GET_WINNER, "%u", p->game.winner);
            } break;

            case MSG_GET_BOARD: {
                i32 res = msendf(in, MSG_GET_BOARD, "%u[%,%]", p->game.board,
                        p->game.board_size, p->game.board_size);
            } break;

            case MSG_GET_PLAYER: {
                msendf(in, MSG_GET_PLAYER, "%u", p->game.current_player);
            } break;
        }
    }

    return PLAYER_OK;
}

int main(int argc, char **argv) {
    if (argc != 7) {
        eprintf("usage: player/1/in player/1/out player/2/in player/2/out board_size timeout_ms\n");
        return 1;
    }
    int p1_in  = open(argv[1], O_WRONLY);
    int p1_out = open(argv[2], O_RDONLY);
    int p2_in  = open(argv[3], O_WRONLY);
    int p2_out = open(argv[4], O_RDONLY);
    u8 board_size = atoi(argv[5]);
    i32 timeout = atoi(argv[6]);
    eprintf("board_size: %d\ntimeout: %d ms\n", (i32)board_size, timeout);

    // TODO(piotr): more robust checking on arguments

    Pentago game;
    PentagoError err = pentago_create(&game, board_size);
    if (err) {
        return 1;
    }

    // initialize a local copy of the game for each player
    Pentago p1_game, p2_game;
    pentago_create(&p1_game, board_size);
    pentago_create(&p2_game, board_size);

    Player p1 = {p1_in, p1_out, p1_game, NULL};
    Player p2 = {p2_in, p2_out, p2_game, NULL};

    HandlePlayerResult res;
    while (!game.winner) {
        res = handle_player(&p1, &game, timeout);
        board_print(&game);
        if (res) {
            printf("ILLEGAL 1\n");
            break;
        }
        if (game.winner) break;
        res = handle_player(&p2, &game, timeout);
        board_print(&game);
        if (res) {
            printf("ILLEGAL 2\n");
            break;
        }
    }
    if (game.winner == 'D') {
        printf("DRAW\n");
    } else {
        printf("WINNER %d\n", (game.winner == 'W') ? 1 : 2);
    }
}
