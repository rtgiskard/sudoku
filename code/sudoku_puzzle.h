#ifndef INCLUDE_SODUKU_PUZZLE_H
#define INCLUDE_SODUKU_PUZZLE_H

#include "sudoku.h"

extern unum puzzle_num;
extern string puzzle_init;
extern struct Mode puzzle_mode;

extern const unum puzzles[][9][9];
extern const unum puzzles_ct;

extern const map<char, Mode_Mthd> map_mthd;
extern const map<char, Mode_Sort> map_sort;
extern const map<char, Mode_Ans> map_ans;

#endif
