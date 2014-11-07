#include "threes.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"
#include "stdio.h"
#include "unistd.h"
#include "assert.h"

int
main(void)
{
	srand(time(NULL));
	board_t *board = comp_new_rand_board();
	char c;
	dir_t dir;

	printf("%d\n", NORTH);
	printf("%d\n", EAST);
	printf("%d\n", WEST);
	printf("%d\n", SOUTH);
	print_board(board);

	int64_t last_score = 0;
	for (int i = 0; ;i++) {
		clock_t start = clock(), diff;
		dir_t next_move = alpha_beta_next_move(board, 10);
		diff = clock() - start;
		int msec = diff * 1000 / CLOCKS_PER_SEC;

		board_t old_board = *board;
	
		player_make_move(board, next_move);
		if (old_board.b == board->b) {
			printf("--------- old board -------\n");
			print_board(&old_board);
			printf("--------- last board ------\n");
			print_board(board);
			printf("DONE\n");
			return 0;
		}
		comp_simulate_turn(board);

		int64_t new_score = board_score(board);
		if (new_score <= last_score) {
			print_board(&old_board);
			printf("-----------------\n");
			print_board(board);
			printf("-----------------\n");
			printf("%d\n", next_move);
			exit(1);
		}
		last_score = new_score;

//		switch (next_move) {
//		case NORTH: printf("---------- NORTH - %d ----------\n", msec); break;
//		case SOUTH: printf("---------- SOUTH - %d ----------\n", msec); break;
//		case EAST:  printf("---------- EAST  - %d ----------\n", msec); break;
//		case WEST:  printf("---------- WEST  - %d ----------\n", msec); break;
//		}
		if (i % 10 == 0) {
			printf("---------\n");
			print_board(board);
		}
	}

	// while not escape sequence
	for (;;) {
		if ((c = getchar()) != 27)
			continue;
		getchar();  // skip [
		switch(getchar()) {
		case 'A':  // up
			dir = NORTH;
			break;
		case 'B':  // down
			dir = SOUTH;
			break;
		case 'C':  // right
			dir = EAST;
			break;
		case 'D':  // left
			dir = WEST;
			break;
		default:
			continue;
		}
		uint64_t old_board = board->b;
		player_make_move(board, dir);
		if (old_board != board->b) {
			comp_simulate_turn(board);
			print_board(board);
			printf("--------\n");
		}
	}
}
