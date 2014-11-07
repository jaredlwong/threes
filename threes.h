#ifndef __THREES_H__
#define __THREES_H__

#include "stdint.h"

struct board {
	uint64_t b		: 64;		// 4 bits * 16 pieces = 64 bits
	uint64_t highest_card	: 32;		// 4 bits needed
	uint64_t last_move_dir	: 32;		// 2 bits needed
	uint64_t		: 40;		// padding
	uint64_t n_ones		: 8;		// 2 bits needed
	uint64_t n_twos		: 8;		// 2 bits needed
	uint64_t n_threes	: 8;		// 2 bits needed
};
typedef struct board board_t;


enum dir {
	NORTH, SOUTH, EAST, WEST
};
typedef enum dir dir_t;


enum piece {
	P0,	P1,	P2,	P3,	P6,
	P12,	P24,	P48,	P96,	P192,
	P384,	P768,	P1536,	P3072,	P6144
};
typedef enum piece piece_t;


#define PIECE_ADD(P1, P2) (value_to_piece[piece_to_value[P1] + piece_to_value[P2]]);

// location to mask, where location = x + 4*y
#define LOC_MASK(I) (0xFULL << (4*(I)))
#define C2I(X, Y) ((uint64_t)(X)+4*(Y))

#define _PIECE(B, I) ((piece_t)(((B) & LOC_MASK(I)) >> (4*(I))))
#define PIECE(B, X, Y) _PIECE(B, C2I(X,Y))

#define _PIECE_NO_SHIFT(B, I) ((B) & LOC_MASK(I))
#define PIECE_NO_SHIFT(B, X, Y) _PIECE_NO_SHIFT(B, C2I(X,Y))

#define _SET_PIECE(BS, I, P) (BS)->b = ((BS)->b & (~LOC_MASK(I))) | ((uint64_t) P) << (4*(I))
#define SET_PIECE(BS, X, Y, P) _SET_PIECE(BS, C2I(X,Y), P)

////////////////////////////////////////////////////////////////////////////////

extern const uint16_t const piece_to_value[];
extern const uint8_t const value_to_piece[];

board_t *create_board(char *bstr[], uint64_t last_move_dir, uint64_t n_ones,
		      uint64_t n_twos, uint64_t n_threes);
board_t parse_board(int argc, char **args);
void print_board(board_t *board);

uint64_t comp_possible_pieces(struct board *board);
uint64_t comp_possible_places(struct board *board);
void comp_make_move(board_t *board, uint64_t place,  piece_t piece);
void comp_simulate_turn(board_t *board);
board_t *comp_new_rand_board(void);

void mov_col_north(board_t *board, uint64_t col);
void mov_col_south(board_t *board, uint64_t col);
void mov_row_west(board_t *board, uint64_t row);
void mov_row_east(board_t *board, uint64_t row);
void player_make_move(board_t *board, dir_t dir);

int64_t board_score(board_t *board);

dir_t alpha_beta_next_move(board_t *board, int depth);

#endif  // __THREES_H__
