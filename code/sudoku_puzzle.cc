#include "sudoku_puzzle.h"

unum puzzle_num = 34;
string puzzle_init = "auto";
struct Mode puzzle_mode = SOLVE_MODE;

const unum puzzles[][9][9] = {
	{	// 0: all 0
		0, 0, 0,   0, 0, 0,   0, 0, 0,
		0, 0, 0,   0, 0, 0,   0, 0, 0,
		0, 0, 0,   0, 0, 0,   0, 0, 0,

		0, 0, 0,   0, 0, 0,   0, 0, 0,
		0, 0, 0,   0, 0, 0,   0, 0, 0,
		0, 0, 0,   0, 0, 0,   0, 0, 0,

		0, 0, 0,   0, 0, 0,   0, 0, 0,
		0, 0, 0,   0, 0, 0,   0, 0, 0,
		0, 0, 0,   0, 0, 0,   0, 0, 0
	},
	{	// 1: only 1 pos filled, may takes seconds to solve in [BC]RF mode
		0, 0, 0,   0, 0, 0,   0, 0, 0,
		0, 0, 0,   0, 0, 0,   0, 0, 0,
		0, 0, 0,   0, 0, 0,   0, 0, 0,

		0, 0, 0,   0, 0, 0,   0, 0, 0,
		0, 0, 0,   0, 7, 0,   0, 0, 0,
		0, 0, 0,   0, 0, 0,   0, 0, 0,

		0, 0, 0,   0, 0, 0,   0, 0, 0,
		0, 0, 0,   0, 0, 0,   0, 0, 0,
		0, 0, 0,   0, 0, 0,   0, 0, 0
	},
	{	// 2: 1 sols
		0, 0, 0,   1, 8, 0,   2, 5, 0,
		0, 0, 0,   5, 0, 0,   0, 0, 0,
		0, 0, 0,   0, 0, 2,   6, 0, 3,

		2, 4, 0,   0, 0, 0,   3, 0, 0,
		7, 0, 0,   0, 9, 0,   0, 0, 5,
		0, 0, 8,   0, 0, 0,   0, 9, 7,

		6, 0, 4,   9, 0, 0,   0, 0, 0,
		0, 0, 0,   0, 0, 3,   0, 0, 0,
		0, 3, 2,   0, 5, 7,   0, 0, 0
	},
	{	// 3: 2 sols
		0, 8, 5,   0, 4, 0,   0, 0, 0,
		0, 0, 0,   2, 0, 0,   5, 0, 0,
		4, 2, 0,   7, 0, 0,   0, 3, 0,

		0, 1, 3,   0, 0, 7,   0, 0, 0,
		2, 5, 4,   0, 9, 0,   0, 0, 3,
		7, 9, 0,   5, 6, 0,   4, 2, 0,

		5, 0, 1,   0, 0, 0,   0, 8, 7,
		8, 0, 0,   0, 0, 0,   0, 0, 4,
		0, 0, 6,   8, 0, 0,   3, 1, 2
	},
	{	// 4: 20 sols
		4, 6, 5,   0, 0, 8,   1, 0, 0,
		2, 3, 8,   1, 0, 0,   4, 6, 0,
		1, 0, 9,   0, 0, 0,   8, 0, 0,

		0, 5, 4,   3, 0, 0,   7, 0, 0,
		0, 0, 0,   8, 0, 0,   0, 0, 0,
		0, 0, 0,   6, 5, 4,   0, 0, 0,

		5, 0, 3,   0, 0, 1,   9, 0, 0,
		0, 0, 0,   9, 0, 0,   0, 4, 3,
		0, 0, 6,   5, 4, 0,   0, 0, 0
	},
	{	// 5: without traverse
		8, 4, 6,   7, 3, 1,   2, 5, 9,
		1, 9, 2,   4, 6, 0,   7, 3, 8,
		3, 5, 7,   2, 9, 8,   4, 1, 6,

		2, 8, 3,   6, 1, 7,   9, 4, 5,
		5, 7, 1,   9, 2, 4,   8, 6, 3,
		9, 6, 4,   5, 0, 3,   1, 7, 2,

		6, 1, 9,   3, 7, 2,   5, 8, 4,
		4, 2, 8,   1, 5, 6,   3, 9, 7,
		7, 3, 5,   8, 4, 9,   6, 2, 1
	},
	{	// 6: 1 sols
		0, 2, 7,   0, 5, 0,   6, 0, 4,
		9, 0, 6,   4, 7, 2,   1, 5, 8,
		5, 4, 0,   9, 8, 0,   7, 0, 2,

		0, 0, 0,   2, 6, 0,   0, 4, 1,
		2, 6, 4,   8, 9, 1,   0, 7, 5,
		1, 8, 5,   3, 0, 7,   2, 6, 9,

		4, 0, 0,   7, 3, 9,   5, 8, 6,
		6, 0, 8,   5, 1, 4,   9, 2, 3,
		3, 5, 9,   0, 2, 0,   4, 1, 7
	},
	{	// 7: no solution
		0, 2, 0,   0, 0, 0,   0, 9, 0,
		0, 7, 0,   0, 0, 9,   0, 3, 6,
		0, 8, 9,   0, 0, 6,   1, 7, 0,

		5, 0, 4,   6, 0, 0,   0, 8, 0,
		3, 0, 2,   1, 0, 0,   0, 0, 4,
		0, 9, 1,   0, 0, 4,   3, 2, 0,

		0, 4, 0,   0, 2, 0,   8, 0, 0,
		9, 0, 0,   0, 0, 0,   0, 0, 3,
		8, 6, 5,   0, 4, 0,   0, 0, 9
	}
};

const unum puzzles_ct = sizeof(puzzles)/sizeof(puzzles[0]);

const map<char, Mode_Mthd> map_mthd = {
	{'C', MTHD_CALC},
	{'B', MTHD_BRUTE},
};
const map<char, Mode_Sort> map_sort = {
	{'S', SORT_SEQ},
	{'F', SORT_FAST},
	{'R', SORT_RAND}
};
const map<char, Mode_Ans> map_ans = {
	{'F', ANS_FRST},
	{'A', ANS_ALL},
};
