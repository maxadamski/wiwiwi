#include "pentago.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


PentagoError pentago_create(Pentago *game, uint8_t size) {
    if (size < 2 || size & 1 || !(size & 2)) {
        return PERR_BAD_ARGUMENT;
    }
    game->board_size = size;
    game->board = (char *)malloc(size*size + 1);
    memset(game->board, '.', size*size);
    game->board[size*size] = 0;

    game->current_player = 'W';
    game->winner = 0;

    return PERR_OK;
}

void pentago_destroy(Pentago *game) {
    free(game->board);
    game->board = NULL;
}

inline char other_player(char player) {
    assert(player == 'B' || player == 'W');
    return (player == 'B') ? 'W' : 'B';
}

char * board_get(Pentago *game, uint8_t i, uint8_t j) {
    return &game->board[i * game->board_size + j];
}

// rotate one layer of the tile
void rotate_square_left(Pentago *game, uint8_t i, uint8_t j, uint8_t size) {
    // for each cycle in the square, rotate the cycle
    for (uint8_t d = 0; d < size-1; d++) {
        char tmp = *board_get(game, i, j+d);
        *board_get(game, i,          j+d)        = *board_get(game, i+d,        j+size-1);
        *board_get(game, i+d,        j+size-1)   = *board_get(game, i+size-1,   j+size-1-d);
        *board_get(game, i+size-1,   j+size-1-d) = *board_get(game, i+size-1-d, j);
        *board_get(game, i+size-1-d, j)          = tmp;
    }
}

// rotate one layer of the tile
void rotate_square_right(Pentago *game, uint8_t i, uint8_t j, uint8_t size) {
    // for each cycle in the square, rotate the cycle
    for (uint8_t d = 0; d < size-1; d++) {
        char tmp = *board_get(game, i, j+d);
        *board_get(game, i,          j+d)        = *board_get(game, i+size-1-d, j);
        *board_get(game, i+size-1-d, j)          = *board_get(game, i+size-1,   j+size-1-d);
        *board_get(game, i+size-1,   j+size-1-d) = *board_get(game, i+d,        j+size-1);
        *board_get(game, i+d,        j+size-1)   = tmp;
    }
}

void board_rotate(Pentago *game, uint8_t rotation) {
    if (rotation > 7) return;
    uint8_t tile_size = game->board_size / 2;
    uint8_t i = (rotation & 4) ? tile_size : 0;
    uint8_t j = (rotation & 2) ? tile_size : 0;

    while (tile_size > 1) {
        // rotate one layer of the tile

        if (rotation & 1) {
            rotate_square_left(game, i, j, tile_size);
        } else {
            rotate_square_right(game, i, j, tile_size);
        }

        // go to next layer inward
        tile_size -= 2;
        i++; j++;
    }
}

PentagoError make_move_(Pentago *game, uint8_t i, uint8_t j, uint8_t rotation, int *no_rotation) {
    // TODO(piotr): more error checking
    if (rotation > 7) {
        return PERR_BAD_ARGUMENT;
    }
    char *field = board_get(game, i, j);
    if (*field != '.' || game->winner) {
        return PERR_ILLEGAL_MOVE;
    }
    *field = game->current_player;
    if (get_winner(game) == game->current_player) {
        game->winner = game->current_player;
        game->current_player = other_player(game->current_player);
        if (no_rotation) *no_rotation = 1;
        return PERR_OK;
    }
    board_rotate(game, rotation);
    game->current_player = other_player(game->current_player);
    game->winner = get_winner(game);
    return PERR_OK;
}

PentagoError make_move(Pentago *game, uint8_t i, uint8_t j, uint8_t rotation) {
    return make_move_(game, i, j, rotation, 0);
}

PentagoError undo_move(Pentago *game, uint8_t i, uint8_t j, uint8_t rotation) {
    if (rotation > 8) {
        return PERR_BAD_ARGUMENT;
    }
    uint8_t reverse_rotation = rotation ^ 1;
    board_rotate(game, reverse_rotation);
    char *field = board_get(game, i, j);
    if (*field != other_player(game->current_player)) {
        board_rotate(game, rotation);
        return PERR_ILLEGAL_MOVE;
    }
    *field = '.';
    game->current_player = other_player(game->current_player);
    game->winner = 0;
    return PERR_OK;
}

// helper function for get_winner
void process_consecutive(uint8_t *count, char *color, uint8_t *white_wins,
        uint8_t *black_wins, uint8_t winning_length, char c) {
    if (c == *color) {
        (*count)++;
        if (*count == winning_length) {
            if (*color == 'W') (*white_wins)++;
            if (*color == 'B') (*black_wins)++;
        }
    } else {
        *color = c;
        *count = 1;
    }
}

char get_winner(Pentago *game) {
    if (game->winner) return game->winner;
    uint8_t size = game->board_size;
    uint8_t winning_length = size/2 + 2;
    uint8_t white_wins = 0;
    uint8_t black_wins = 0;
    // check rows
    for (int32_t i = 0; i < size; i++) {
        uint8_t count = 0;
        char color = '.';
        for (int32_t j = 0; j < size; j++) {
            process_consecutive(&count, &color, &white_wins, &black_wins, winning_length,
                    *board_get(game, (uint8_t)i, (uint8_t)j));
        }
    }
    // check columns
    for (int32_t j = 0; j < size; j++) {
        uint8_t count = 0;
        char color = '.';
        for (int32_t i = 0; i < size; i++) {
            process_consecutive(&count, &color, &white_wins, &black_wins, winning_length,
                    *board_get(game, (uint8_t)i, (uint8_t)j));
        }
    }
    // check / diagonals, top-left half
    for (int32_t i = winning_length-1; i < size; i++) {
        uint8_t count = 0;
        char color = '.';
        for (int32_t j = 0; i-j >= 0; j++) {
            process_consecutive(&count, &color, &white_wins, &black_wins, winning_length,
                    *board_get(game, (uint8_t)(i-j), (uint8_t)j));
        }
    }
    // check / diagonals, bottom-right half
    for (int32_t j = 0; j <= size - winning_length; j++) {
        uint8_t count = 0;
        char color = '.';
        // here i is the offset from the bottom edge of the board
        for (int32_t i = 0; i+j < size; i++) {
            process_consecutive(&count, &color, &white_wins, &black_wins, winning_length,
                    *board_get(game, (uint8_t)(size-1-i), (uint8_t)(i+j)));
        }
    }
    // check \ diagonals, bottom-left half
    for (int32_t i = 0; i <= size - winning_length; i++) {
        uint8_t count = 0;
        char color = '.';
        for (int32_t j = 0; i+j < size; j++) {
            process_consecutive(&count, &color, &white_wins, &black_wins, winning_length,
                    *board_get(game, (uint8_t)(i+j), (uint8_t)j));
        }
    }
    // check \ diagonals, top-right half
    for (int32_t j = 0; j <= size - winning_length; j++) {
        uint8_t count = 0;
        char color = '.';
        for (int32_t i = 0; i+j < size; i++) {
            process_consecutive(&count, &color, &white_wins, &black_wins, winning_length,
                    *board_get(game, (uint8_t)i, (uint8_t)(i+j)));
        }
    }

    char result = 0;
    if      (white_wins && black_wins)  result = 'D';
    else if (white_wins && !black_wins) result = 'W';
    else if (!white_wins && black_wins) result = 'B';
    else {
        int32_t empty_fields = 0;
        for (int32_t i = 0; i < size; i++) {
            for (int32_t j = 0; j < size; j++) {
                if (*board_get(game, i, j) == '.') {
                    empty_fields++;
                    break;
                }
            }
            if (empty_fields) break;
        }
        if (empty_fields == 0) result = 'D';
    }

    return result;
}

void board_print(Pentago *game) {
    for (uint8_t i = 0; i < game->board_size; i++) {
        for (uint8_t j = 0; j < game->board_size; j++) {
            putc(*board_get(game, i, j), stderr);
        }
        putc('\n', stderr);
    }
}

PentagoMoves get_available_moves(Pentago *game) {
    if (game->winner) {
        // the game is finished, can't make any moves
        return (PentagoMoves){0};
    }

    // first pass to count empty fields
    int32_t moves_available = 0;
    for (int32_t i = 0; i < game->board_size * game->board_size; i++) {
        // for each empty field there are 8 possible rotations
        if (game->board[i] == '.') moves_available += 8;
    }

    uint8_t *result_mem = malloc(moves_available * 3);
    PentagoMoves result;
    result.count    = moves_available;
    result.i        = result_mem;
    result.j        = result_mem + moves_available;
    result.rotation = result_mem + moves_available * 2;

    uint8_t all_rotations[] = {0, 1, 2, 3, 4, 5, 6, 7};
    int32_t moves_filled = 0;
    for (uint8_t i = 0; i < game->board_size; i++) {
        for (uint8_t j = 0; j < game->board_size; j++) {
            if (*board_get(game, i, j) == '.') {
                memset(result.i + moves_filled, i, 8);
                memset(result.j + moves_filled, j, 8);
                memcpy(result.rotation + moves_filled, all_rotations, 8);
                moves_filled += 8;
            }
        }
    }

    assert(moves_available == moves_filled);
    return result;
}

void free_moves(PentagoMoves *moves) {
    free(moves->i);
    *moves = (PentagoMoves){0};
}
