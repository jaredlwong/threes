#include "threes.h"
#include "stdlib.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "limits.h"


const uint16_t const piece_to_value[] = {
	[P0]	= 0,
	[P1]	= 1,
	[P2]	= 2,
	[P3]	= 3,
	[P6]	= 6,
	[P12]	= 12,
	[P24]	= 24,
	[P48]	= 48,
	[P96]	= 96,
	[P192]	= 192,
	[P384]	= 384,
	[P768]	= 768,
	[P1536]	= 1536,
	[P3072]	= 3072,
	[P6144]	= 6144
};


const uint8_t const value_to_piece[] = {
	[0]	= P0,
	[1]	= P1,
	[2]	= P2,
	[3]	= P3,
	[6]	= P6,
	[12]	= P12,
	[24]	= P24,
	[48]	= P48,
	[96]	= P96,
	[192]	= P192,
	[384]	= P384,
	[768]	= P768,
	[1536]	= P1536,
	[3072]	= P3072,
	[6144]	= P6144
};

const char * const dir_to_str[] = {
	[NORTH]	= "NORTH",
	[SOUTH]	= "SOUTH",
	[EAST]	= "EAST",
	[WEST]	= "WEST"
};


board_t *
create_board(char *bstr[], uint64_t last_move_dir, uint64_t n_ones,
	     uint64_t n_twos, uint64_t n_threes)
{
	board_t b = parse_board(16, bstr);
	board_t *board = (board_t *) malloc(sizeof(board_t));
	memset(board, 0, sizeof(board_t));
	*board = b;
	board->last_move_dir = last_move_dir;
	board->n_ones = n_ones;
	board->n_twos = n_twos;
	board->n_threes = n_threes;
	return board;
}


board_t
parse_board(int argc, char **args)
{
	board_t board;
	memset(&board, 0, sizeof(board_t));
	int i;
	piece_t p;

	if (argc != 16) {
		printf("Need 16 pieces\n");
		exit(1);
	}
	for (i = 0; i < 16; i++) {
		p = value_to_piece[atoi(args[i])];
		if (p > board.highest_card)
			board.highest_card = p;
		_SET_PIECE(&board, i, p);
	}
	return board;
}

void
print_board(board_t *board)
{
	uint64_t x, y;
	for (y = 0; y < 4; y++) {
		for (x = 0; x < 3; x++) {
			printf("%llu\t", piece_to_value[PIECE(board->b, x, y)]);
                }
		printf("%llu\n", piece_to_value[PIECE(board->b, x, y)]);
	}
}

// There are a maximum of 11 cards, P1 to P768
// returns bit array of which pieces can be places where the ith bit represents
// the ith piece
uint64_t
comp_possible_pieces(struct board *board)
{
	uint64_t possible = 0, i;
	if (board->highest_card >= P48) {
		for (i = P48; i <= board->highest_card; i++) {
			possible |= 1 << value_to_piece[piece_to_value[i]/8];
                }
	}
	if (board->n_ones > 0)
		possible |= 1 << P1;
	if (board->n_twos > 0)
		possible |= 1 << P2;
	if (board->n_threes > 0)
		possible |= 1 << P3;
	return possible;
}

// return bit array of which places, by index, are free on a side based on the
// last move direction
uint64_t
comp_possible_places(struct board *board)
{
	uint64_t possible = 0;
	switch(board->last_move_dir) {
	case NORTH:
		if (PIECE_NO_SHIFT(board->b, 0, 3) == 0) possible |= 1 << C2I(0, 3);
		if (PIECE_NO_SHIFT(board->b, 1, 3) == 0) possible |= 1 << C2I(1, 3);
		if (PIECE_NO_SHIFT(board->b, 2, 3) == 0) possible |= 1 << C2I(2, 3);
		if (PIECE_NO_SHIFT(board->b, 3, 3) == 0) possible |= 1 << C2I(3, 3);
		break;
	case SOUTH:
		if (PIECE_NO_SHIFT(board->b, 0, 0) == 0) possible |= 1 << C2I(0, 0);
		if (PIECE_NO_SHIFT(board->b, 1, 0) == 0) possible |= 1 << C2I(1, 0);
		if (PIECE_NO_SHIFT(board->b, 2, 0) == 0) possible |= 1 << C2I(2, 0);
		if (PIECE_NO_SHIFT(board->b, 3, 0) == 0) possible |= 1 << C2I(3, 0);
		break;
	case EAST:
		if (PIECE_NO_SHIFT(board->b, 0, 0) == 0) possible |= 1 << C2I(0, 0);
		if (PIECE_NO_SHIFT(board->b, 0, 1) == 0) possible |= 1 << C2I(0, 1);
		if (PIECE_NO_SHIFT(board->b, 0, 2) == 0) possible |= 1 << C2I(0, 2);
		if (PIECE_NO_SHIFT(board->b, 0, 3) == 0) possible |= 1 << C2I(0, 3);
		break;
	case WEST:
		if (PIECE_NO_SHIFT(board->b, 3, 0) == 0) possible |= 1 << C2I(3, 0);
		if (PIECE_NO_SHIFT(board->b, 3, 1) == 0) possible |= 1 << C2I(3, 1);
		if (PIECE_NO_SHIFT(board->b, 3, 2) == 0) possible |= 1 << C2I(3, 2);
		if (PIECE_NO_SHIFT(board->b, 3, 3) == 0) possible |= 1 << C2I(3, 3);
		break;
	}
	return possible;
}

// piece should be an enum piece, and place should be an index into the board
// [0, 16)
void
comp_make_move(board_t *board, uint64_t place, piece_t piece)
{
	switch (piece) {
	case P1:
		board->n_ones -= 1;
		break;
	case P2:
		board->n_twos -= 1;
		break;
	case P3:
		board->n_threes -= 1;
		break;
	}

	// if all of the normal pieces have been used, reset
	uint64_t *ba = (uint64_t *) board;
	if (((uint64_t *)board)[2] == 0) {
		board->n_ones = 4;
		board->n_twos = 4;
		board->n_threes = 4;
	}

	// select everything except place, and put piece in place
	_SET_PIECE(board, place, piece);
}

// make a simulated move based on
// http://forums.toucharcade.com/showpost.php?p=3140044&postcount=522
void
comp_simulate_turn(board_t *board)
{
	piece_t next_piece;
	piece_t possible[12];
	uint64_t next_place;
	int i, j, r;
	uint64_t possible_places;

	if (board->highest_card >= P48 && (rand() % 21) == 20) {
		// subtract to divide the value by 8
		next_piece = P48 + (rand() % (board->highest_card - P48 + 1)) - 3;
	} else {
		for (i = 0; i < board->n_ones; i++) possible[i] = P1;
		for (     ; i < board->n_twos; i++) possible[i] = P2;
		for (     ; i < board->n_threes; i++) possible[i] = P3;
		next_piece = possible[rand() % i];
	}

	possible_places = comp_possible_places(board);
	int total = 0;
	for (i = 0; i < 16; i++) total += (possible_places & (1 << i)) ? 1 : 0;
	r = rand() % total;
	j = 0;
	for (i = 0; i < 16; i++) {
		if (possible_places & (1 << i)) {
			if (j++ == r) {
				next_place = i;
			}
		}
	}

	comp_make_move(board, next_place, next_piece);
}

board_t *
comp_new_rand_board(void)
{
	char *bstr[] = {
		"0", "0", "0", "0",
		"0", "0", "0", "0",
		"0", "0", "0", "0",
		"0", "0", "0", "0"
	};

	board_t *board = create_board(bstr, NORTH, 0, 0, 0);

	// place 12 randomly
	int i = 12;
	piece_t pieces_to_place[] = {
		P1, P1, P1, P1,
		P2, P2, P2, P2,
		P3, P3, P3, P3
	};
	while (i > 0) {
		uint64_t place = rand() % 16;
		if (_PIECE(board->b, place) == P0) {
			_SET_PIECE(board, place, pieces_to_place[12-i]);
			i -= 1;
		}
	}

	// remove three randomly
	i = 3;
	while (i > 0) {
		uint64_t place = rand() % 16;
		piece_t piece = _PIECE(board->b, place);
		if (piece > P0) {
			_SET_PIECE(board, place, P0);
			switch (piece) {
			case P1: board->n_ones += 1; break;
			case P2: board->n_twos += 1; break;
			case P3: board->n_threes += 1; break;
			}
			i -= 1;
		}
	}

	return board;
}

void
mov_col_north(board_t *board, uint64_t col)
{
	uint64_t a = PIECE(board->b, col, 0);
	uint64_t b = PIECE(board->b, col, 1);
	uint64_t c = PIECE(board->b, col, 2);
	uint64_t d = PIECE(board->b, col, 3);
	if (a == 0 || (a == b && a >= P3) || a + b == 3) { a = PIECE_ADD(a, b); b = P0; }
	if (b == 0 || (b == c && b >= P3) || b + c == 3) { b = PIECE_ADD(b, c); c = P0; }
	if (c == 0 || (c == d && c >= P3) || c + d == 3) { c = PIECE_ADD(c, d); d = P0; }
	SET_PIECE(board, col, 0, a);
	SET_PIECE(board, col, 1, b);
	SET_PIECE(board, col, 2, c);
	SET_PIECE(board, col, 3, d);
}

void
mov_col_south(board_t *board, uint64_t col)
{
	uint64_t a = PIECE(board->b, col, 3);
	uint64_t b = PIECE(board->b, col, 2);
	uint64_t c = PIECE(board->b, col, 1);
	uint64_t d = PIECE(board->b, col, 0);
	if (a == 0 || (a == b && a >= P3) || a + b == 3) { a = PIECE_ADD(a, b); b = P0; }
	if (b == 0 || (b == c && b >= P3) || b + c == 3) { b = PIECE_ADD(b, c); c = P0; }
	if (c == 0 || (c == d && c >= P3) || c + d == 3) { c = PIECE_ADD(c, d); d = P0; }
	SET_PIECE(board, col, 3, a);
	SET_PIECE(board, col, 2, b);
	SET_PIECE(board, col, 1, c);
	SET_PIECE(board, col, 0, d);
}

void
mov_row_west(board_t *board, uint64_t row)
{
	uint64_t a = PIECE(board->b, 0, row);
	uint64_t b = PIECE(board->b, 1, row);
	uint64_t c = PIECE(board->b, 2, row);
	uint64_t d = PIECE(board->b, 3, row);
	if (a == 0 || (a == b && a >= P3) || a + b == 3) { a = PIECE_ADD(a, b); b = P0; }
	if (b == 0 || (b == c && b >= P3) || b + c == 3) { b = PIECE_ADD(b, c); c = P0; }
	if (c == 0 || (c == d && c >= P3) || c + d == 3) { c = PIECE_ADD(c, d); d = P0; }
	SET_PIECE(board, 0, row, a);
	SET_PIECE(board, 1, row, b);
	SET_PIECE(board, 2, row, c);
	SET_PIECE(board, 3, row, d);
}

void
mov_row_east(board_t *board, uint64_t row)
{
	uint64_t a = PIECE(board->b, 3, row);
	uint64_t b = PIECE(board->b, 2, row);
	uint64_t c = PIECE(board->b, 1, row);
	uint64_t d = PIECE(board->b, 0, row);
	if (a == 0 || (a == b && a >= P3) || a + b == 3) { a = PIECE_ADD(a, b); b = P0; }
	if (b == 0 || (b == c && b >= P3) || b + c == 3) { b = PIECE_ADD(b, c); c = P0; }
	if (c == 0 || (c == d && c >= P3) || c + d == 3) { c = PIECE_ADD(c, d); d = P0; }
	SET_PIECE(board, 3, row, a);
	SET_PIECE(board, 2, row, b);
	SET_PIECE(board, 1, row, c);
	SET_PIECE(board, 0, row, d);
}

void
player_make_move(board_t *board, dir_t dir)
{
	int i;
	piece_t p;
	switch (dir) {
	case NORTH:
		for (i = 0; i < 4; i++) mov_col_north(board, i);
		break;
	case SOUTH:
		for (i = 0; i < 4; i++) mov_col_south(board, i);
		break;
	case EAST:
		for (i = 0; i < 4; i++) mov_row_east(board, i);
		break;
	case WEST:
		for (i = 0; i < 4; i++) mov_row_west(board, i);
		break;
	}
	board->last_move_dir = dir;
	for (i = 0; i < 16; i++) {
		p = _PIECE(board->b, i);
		if (board->highest_card < p)
			board->highest_card = p;
	}
}

int64_t
board_score(board_t *board)
{
	int64_t score = 0;
	for (int i = 0; i < 16; i++) {
		score += piece_to_value[_PIECE(board->b, i)];
	}
	return score;
}

int64_t
eval_board(board_t *board)
{
	int64_t num_empty = 0;		// 0
	int64_t num_pairs = 0;		// 3, 3
	int64_t num_bad_pairs = 0;	// 1, 1 || 2, 2
	int64_t num_double_pairs = 0;	// x, 2*x
	int64_t num_bad_position = 0;

	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++) {
			piece_t neighbors[4] = {
				(y-1 > 0) ? PIECE(board->b, x, y-1) : P0,
				(y+1 < 4) ? PIECE(board->b, x, y+1) : P0,
				(x-1 > 0) ? PIECE(board->b, x-1, y) : P0,
				(x+1 < 4) ? PIECE(board->b, x+1, y) : P0
			};
			piece_t cur = PIECE(board->b, x, y);
			if (cur == 0) {
				num_empty += 1;
			} else {
				int64_t num_more = 0;
				for (int i = 0; i < 4; i++) {
					if (cur <= P2) {
						if (neighbors[i] + cur == 3) {
							num_pairs += 1;
						} else if (neighbors[i] == cur) {
							num_bad_pairs += 1;
						}
					} else {
						if (neighbors[i] == cur) {
							num_pairs += 1;
						} else if (neighbors[i] == cur + 1) {
							num_double_pairs += 1;
						}
						if (neighbors[i] > cur) {
							num_more += 1;
						}
					}
				}
				if (num_more >= 2 || (num_more >= 1 && (x == 0 || x == 3 || y == 0 || y == 3))) {
					num_bad_position += 1;
				}
			}
		}
	}

	if (num_empty == 0 && num_pairs == 0) {
		return -1000;
	}
	
	return num_empty * 2 + num_pairs * 2 + num_double_pairs - num_bad_position;
}


int64_t
alpha_beta_helper(board_t *board, int depth, int64_t alpha, int64_t beta, int is_player)
{
	if (depth == 0)
		return eval_board(board);
	if (is_player) {
		board_t new_boards[4];
		int any_valid = 0;
		dir_t move;
		for (move = 0; move < 4; move++) {
			new_boards[move] = *board;
			player_make_move(new_boards+move, move);
			// illegal move
			if (new_boards[move].b == board->b) {
				memset(new_boards+move, 0, sizeof(board_t));
				continue;
			}
			any_valid = 1;
		}
		if (!any_valid)
			return eval_board(board);
		for (move = 0; move < 4; move++) {
			if (new_boards[move].b == 0)
				continue;
			int64_t score = -1 * alpha_beta_helper(new_boards+move, depth-1, -1 * beta, -1 * alpha, 0);
			if (score > alpha)
				alpha = score;
			if (alpha >= beta)
				return alpha;
		}
	} else {
		uint64_t possible_pieces = comp_possible_pieces(board);
		uint64_t possible_places = comp_possible_places(board);
		int is_valid = 0;
		for (piece_t i = P1; i <= P768; i++) {
			if ((possible_pieces & (1ULL << i)) == 0)
				continue;
			for (int j = 0; j < 16; j++) {
				if ((possible_places & (1ULL << j)) == 0)
					continue;
				is_valid = 1;
				board_t new_board = *board;
				comp_make_move(&new_board, j, i);
				int64_t score = -1 * alpha_beta_helper(&new_board, depth-1, -1 * beta, -1 * alpha, 1);
				if (score > alpha)
					alpha = score;
				if (alpha >= beta)
					return alpha;
			}
		}
		if (!is_valid) {
			printf("ERRROR NOT VALID\n");
			exit(1);
		}
	}
	return alpha;
}

dir_t
alpha_beta_next_move(board_t *board, int depth)
{
	int have_move = 0;
	int64_t best_score = -1000;
	dir_t best_move = -1;
	dir_t move;
	for (move = 0; move < 4; move++) {
		board_t new_board = *board;
		player_make_move(&new_board, move);
		// illegal move
		if (new_board.b == board->b) continue;
		int64_t score = -1 * alpha_beta_helper(&new_board, depth-1, -1000, 1000, 0);
		if (have_move == 0 || score > best_score) {
			best_score = score;
			best_move = move;
			have_move = 1;
		}
	}
	if (best_move == -1) {
		printf("FUCK FUCK\n");
		print_board(board);
	} else {
		printf("move: %s\tscore: %lld\thave move: %d\n", dir_to_str[best_move], best_score, have_move);
	}
	return best_move;
}
