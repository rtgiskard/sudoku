#ifndef INCLUDE_SUDOKU_H
#define INCLUDE_SUDOKU_H

#include <iostream>
#include <iomanip>
#include <ctime>
#include <string>
#include <cstring>
#include <map>
#include <vector>

using namespace std;

#define MAX_SOLS_CT		2e4
#define SOLVE_MODE		{MTHD_CALC, SORT_FAST, ANS_ALL}

#define DEBUG_LEVEL		DBGL_NONE

// debug level
#define DBGL_NONE		0
#define DBGL_DETAIL		1
#define DBGL_TRACE		2

// misc define
#define CT_D1			9			// count for 1 dimension
#define CT_D2			(9*9)		// count for 2 dimension
#define CT_D3			(9*9*9)		// count for 3 dimension
#define CT_ERR			(CT_D3 + 1)	// if reached this count, considered ERR

#define UFIL			'.'			// char to represent unfilled position

// adjust random seed manually for debug
#if (DEBUG_LEVEL > DBGL_NONE)
	#define DBG_RAND_SEED	200
#else
	#define DBG_RAND_SEED	(time(NULL) + rand())
#endif

// for compiler compatibility
#if defined(__clang__)
	#define FALLTHROUGH [[clang::fallthrough]]
#elif defined(__GNUC__)
	#define FALLTHROUGH __attribute__ ((fallthrough))
#endif

typedef unsigned short	unum;
typedef unsigned int	uint;
typedef unsigned long	ulong;

//<1 mode related
enum Mode_Mthd {	// entry: check_and_fill()
	MTHD_CALC,		// solve by analyze
	MTHD_BRUTE		// solve by brute force
};
enum Mode_Sort {	// entry: init_pos_state(), sort_vect
	SORT_SEQ,		// force seq sort
	SORT_RAND,		// force random sort
	SORT_FAST		// fast sort, only switch if 0 found
};
enum Mode_Ans {		// entry: traverse_state()
	ANS_FRST,		// find the first answer
	ANS_ALL			// find all answer
};

struct Mode {
	Mode_Mthd mthd;
	Mode_Sort sort;
	Mode_Ans ans;
};

typedef struct _Pos_Node {	//<1
	unum id;		// id= i * 9 + j @ar[i][j]
	unum ck;		// current index of selected candidate
	unum * p_ar;	// ptr to num in ar[i][j]
	unum * p_ct;	// ptr to ct in ar_pos_ct[i][j]
	unum * avail;	// candidates for the position
} Pos_Node;

class SuDoKu { //<1
  private:
	unum		ar[9][9];				// nums in the position
	unum		ar_pos_ct[9][9];		// counter of the choices for every position
	unum		ar_pos_avail[9][9][9];	// list of available nums for every position
	Pos_Node	pos_state[CT_D2];		// the state tree for evry position

	struct Mode mode;					// mode to solve the puzzle
	vector<unum (*)[9]> sols;			// hold the solutions

  public:
	SuDoKu(string init = "auto", unum ct = 20, struct Mode mode = SOLVE_MODE);
	~SuDoKu();

	void show_ar();
	bool try_solve();

  protected:
	// for debug: call with gdb
	void dbg_show_pos_ct();
	void dbg_show_pos_state();

	void save_sol();
	void clear_sol();
	void show_sol();

	bool gen_ar(unum ct);
	bool check_ar(bool allow_zero = false);

	void show_ar_x(unum ar_x[9][9], string prompt = "");
	void show_pos_state(Pos_Node src[], string prompt = "");

	unum trim_for_related_rcg(unum i, unum j);
	bool update_for_related_rcg(unum i, unum j);
	bool update_pos_avail_and_ct_at_idx(unum i, unum j);
	unum sync_pos_avail_and_ct(unum seq = 0);
	unum ar_commit_from_state(unum start = 0, unum end = CT_D2);
	void set_pos_state(unum seq, unum ck, unum i, unum j);
	unum init_pos_state();

	bool fill_check_for_calc(unum seq);
	bool fill_check_for_brute(unum seq);
	bool check_and_fill(unum seq);

	bool traverse_state(unum seq_root);

	inline void reset_sub_state_ck(unum seq)
	{
		for (unum i = seq; i < CT_D2; i++)
			pos_state[i].ck = *pos_state[i].p_ct;
	}

	inline unum idx_gi(unum n, unum k)   // row of the elem k in group n
	{
		return n / 3 * 3 + k / 3;
	}
	inline unum idx_gj(unum n, unum k)   // col of the elem k in group n
	{
		return n % 3 * 3 + k % 3;
	}
	inline unum idx_gn(unum i, unum j)   // group id for ar[i][j]
	{
		return i / 3 * 3 + j / 3;
	}
	inline unum seq_gn(unum i, unum j)   // seq in group for ar[i][j]
	{
		return i % 3 * 3 + j % 3;
	}
}; //>

#endif
