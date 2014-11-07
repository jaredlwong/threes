#include "threes.h"
#include "stdint.h"
#include "assert.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

void
test_possible_pieces(void)
{
	board_t *board;
	uint64_t possible_pieces;
	char *bstr[] = {
		"0", "0", "0", "0",
		"0", "0", "0", "0",
		"0", "0", "0", "0",
		"0", "0", "0", "0"
	};

	board = create_board(bstr, 0, 0, 0, 0);
	possible_pieces = comp_possible_pieces(board);
	assert((possible_pieces & (1 << P0)) == 0);
	assert((possible_pieces & (1 << P1)) == 0);
	assert((possible_pieces & (1 << P2)) == 0);
	assert((possible_pieces & (1 << P3)) == 0);
	free(board);

	board = create_board(bstr, 0, 4, 4, 4);
	possible_pieces = comp_possible_pieces(board);
	assert((possible_pieces & (1 << P0)) == 0);
	assert((possible_pieces & (1 << P1)) > 0);
	assert((possible_pieces & (1 << P2)) > 0);
	assert((possible_pieces & (1 << P3)) > 0);
	assert(board->n_ones == 4);
	assert(board->n_twos == 4);
	assert(board->n_threes == 4);
	free(board);

	bstr[0] = "768";
	board = create_board(bstr, 0, 4, 4, 4);
	possible_pieces = comp_possible_pieces(board);
	assert((possible_pieces & (1 << P0)) == 0);
	assert((possible_pieces & (1 << P1)) > 0);
	assert((possible_pieces & (1 << P2)) > 0);
	assert((possible_pieces & (1 << P3)) > 0);
	assert((possible_pieces & (1 << P6)) > 0);
	assert((possible_pieces & (1 << P12)) > 0);
	assert((possible_pieces & (1 << P24)) > 0);
	assert((possible_pieces & (1 << P48)) > 0);
	assert((possible_pieces & (1 << P96)) > 0);
	assert((possible_pieces & (1 << P192)) == 0);
	free(board);
}

void
test_possible_places(void)
{
	board_t *board;
	uint64_t possible_places;
	char *bstr[] = {
		"0", "0", "0", "0",
		"0", "0", "0", "0",
		"0", "0", "0", "0",
		"0", "0", "0", "0"
	};

	board = create_board(bstr, NORTH, 0, 0, 0);
	possible_places = comp_possible_places(board);
	assert(board->last_move_dir == NORTH);
	assert((possible_places & (1 << C2I(0,3))) > 0);
	assert((possible_places & (1 << C2I(1,3))) > 0);
	assert((possible_places & (1 << C2I(2,3))) > 0);
	assert((possible_places & (1 << C2I(3,3))) > 0);
	free(board);
}


void
test_comp_make_move(void)
{
	board_t *board;
	char *bstr[] = {
		"0", "0", "0", "0",
		"0", "0", "0", "0",
		"0", "0", "0", "0",
		"0", "0", "0", "0"
	};

	board = create_board(bstr, NORTH, 4, 4, 4);
	comp_make_move(board, C2I(0, 3), P1);
	assert(PIECE(board->b, 0, 3) == P1);
	assert(board->n_ones == 3);
	free(board);

	board = create_board(bstr, NORTH, 0, 0, 1);
	comp_make_move(board, C2I(0, 3), P3);
	assert(PIECE(board->b, 0, 3) == P3);
	assert(board->n_ones == 4);
	free(board);
}

void
test_player_make_move(void)
{
	board_t *board, *eboard;
	char *bstr[] = {
		"1", "0", "3", "3",
		"2", "1", "3", "6",
		"0", "2", "0", "6",
		"0", "0", "0", "0"
	};
	char *bstr_expected[] = {
		"3", "1", "6", "3",
		"0", "2", "0", "12",
		"0", "0", "0", "0",
		"0", "0", "0", "0"
	};
	int x, y;

	board = create_board(bstr, NORTH, 4, 4, 4);
	eboard = create_board(bstr_expected, NORTH, 4, 4, 4);
	player_make_move(board, NORTH);
	for (y = 0; y < 4; y++) {
		for (x = 0; x < 4; x++) {
			assert(PIECE(board->b, x, y) == PIECE(eboard->b, x, y));
		}
	}
	free(board);
}

void
assert_row0(board_t *board, piece_t A, piece_t B, piece_t C, piece_t D)
{
	assert(PIECE(board->b, 0, 0) == A);
	assert(PIECE(board->b, 1, 0) == B);
	assert(PIECE(board->b, 2, 0) == C);
	assert(PIECE(board->b, 3, 0) == D);
}

void
test_move_row_west(void)
{
	board_t *board;

	char *bstr1[] = { "1", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0" };
	board = create_board(bstr1, NORTH, 4, 4, 4);
	mov_row_west(board, 0);
	assert_row0(board, P1, P0, P0, P0);
	free(board);

	char *bstr2[] = { "1", "1", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0" };
	board = create_board(bstr2, NORTH, 4, 4, 4);
	mov_row_west(board, 0);
	assert_row0(board, P1, P1, P0, P0);
	free(board);

	char *bstr3[] = { "1", "2", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0" };
	board = create_board(bstr3, NORTH, 4, 4, 4);
	mov_row_west(board, 0);
	assert_row0(board, P3, P0, P0, P0);
	free(board);

	char *bstr4[] = { "1", "2", "2", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0" };
	board = create_board(bstr4, NORTH, 4, 4, 4);
	mov_row_west(board, 0);
	assert_row0(board, P3, P2, P0, P0);
	free(board);

	char *bstr5[] = { "2", "2", "2", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0" };
	board = create_board(bstr5, NORTH, 4, 4, 4);
	mov_row_west(board, 0);
	assert_row0(board, P2, P2, P2, P0);
	free(board);
}



int
main(int argc, char **argv)
{
	test_possible_pieces();
	test_possible_places();
	test_comp_make_move();
	test_player_make_move();
	test_move_row_west();
}
