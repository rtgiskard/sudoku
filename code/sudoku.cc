#include "sudoku.h"
#include "sudoku_func.h"
#include "sudoku_puzzle.h"

SuDoKu::SuDoKu(string init, unum ct, struct Mode mode): mode(mode) //<1
{
	if (init == "auto")	// or generate random puzzle
		gen_ar(ct);
	else				// use predefined puzzle
		memcpy(ar, puzzles[puzzle_num], sizeof(ar));
}

SuDoKu::~SuDoKu() //<1
{
	clear_sol();
}

bool SuDoKu::gen_ar(unum ct) //<1
{
	/**
	 * generate puzzle：
	 *	1. generate an empty sudoku
	 *	2. solve it with CRF mode
	 *	3. mask several pos of the answer, and that's the puzzle
	 */

	// fill with 0 by default
	memset(ar, 0, sizeof(ar));

	srand(DBG_RAND_SEED);

	// solve the empty sudoku via CRF mode
	// need backup and recover the original mode
	struct Mode mode_orig = mode;				// backup original mode
	mode =  {MTHD_CALC, SORT_RAND, ANS_FRST};	// use CRF mode to initial puzzle
	sync_pos_avail_and_ct();					// sync pos_ct and pos_avail from ar
	traverse_state(init_pos_state());			// init pos_state and begin traverse
	mode = mode_orig;							// recover original mode
	clear_sol();								// clear sols

	srand(DBG_RAND_SEED);

	// postion index with random order
	unum rand_seq[CT_D2];

	for (unum i = 0; i < CT_D2; i++) // init rand_seq
		rand_seq[i] = i;

	for (unum i = 0; i < CT_D2; i++) // random switch in rand_seq
		switch_ab(rand_seq[i], rand_seq[rand() % CT_D2]);

	for (unum i = 0; i < ct; i++)	// mask the first ct in rand_seq
		ar[rand_seq[i] / 9][rand_seq[i] % 9] = 0;

	return true;
}

bool SuDoKu::check_ar(bool allow_zero) //<1
{
	/**
	 * check whether the sudoku is solved
	 * 1. no 0 if allow_zero = false
	 * 2. no repetition: loop by row/col/group
	 */

	for (unum n = 0; n < 9; n++) {
		for (unum i = 0; i < 9; i++) {

			// index of group[n][i]
			unum g_i = idx_gi(n, i);
			unum g_j = idx_gj(n, i);

			if (!allow_zero && ( ar[n][i] == 0 || ar[i][n] == 0 || ar[g_i][g_j] == 0 ))
				return false;

			if (ar[n][i] != 0)
				for (unum k = i + 1; k < 9; k++)	// row
					if (ar[n][i] == ar[n][k])
						return false;

			if (ar[i][n] != 0)
				for (unum k = i + 1; k < 9; k++)	// col
					if (ar[i][n] == ar[k][n])
						return false;

			if (ar[g_i][g_j] != 0)
				for (unum k = i + 1; k < 9; k++)	// group
					if (ar[g_i][g_j] == ar[idx_gi(n, k)][idx_gj(n, k)])
						return false;
		}
	}

	return true;
} //>

void SuDoKu::save_sol() //<1
{
	unum (*ptr)[9] = new unum[9][9];
	memcpy(ptr, ar, sizeof(ar));
	sols.push_back(ptr);
}

void SuDoKu::clear_sol() //<1
{
	for (auto psol : sols)
		delete []psol;

	sols.clear();
}

void SuDoKu::show_sol() //<1
{
	for (unum i = 0; i < sols.size(); i++)
		// as no other place to save the ar, just print it
		show_ar_x(sols[i], "solution[" + to_string(i + 1) + "/" + to_string(sols.size()) + "]:");
}

void SuDoKu::show_ar() //<1
{
	show_ar_x(ar, "sudoku:");
}

void SuDoKu::dbg_show_pos_ct() //<1
{
	show_ar_x(ar_pos_ct, "ar_pos_ct:");
}

void SuDoKu::dbg_show_pos_state() //<1
{
	show_pos_state(pos_state, "pos_state:");
}

void SuDoKu::show_ar_x(unum ar_x[9][9], string prompt) //<1
{
	if (prompt.length() != 0)
		cout << prompt << '\n';

	for (unum i = 0; i < 9; i++) {

		// cout line group by 3 rows, also endl before the 1st line
		if (i % 3 == 0)
			cout << '\n';

		// cout columes
		for (unum j = 0; j < 9; j++) {

			// cout x for unfilled position
			if (ar_x[i][j] == 0)
				cout << right << setw(3) << UFIL;
			else
				cout << right << setw(3) << ar_x[i][j];

			// cout group spacer and endl
			if (j == 8)
				cout << '\n';
			else if (j % 3 == 2)
				cout << right << setw(2) << "";
		}
	}

	cout << endl;
}

void SuDoKu::show_pos_state(Pos_Node src[], string prompt) //<1
{
	if (prompt.length() != 0)
		cout << prompt << '\n';

	for (unum i = 0; i < CT_D2; i++) {
		cout << right << setw(4) << i
		    << " (" << src[i].id / 9 << "," << src[i].id % 9 << ")  "
		    << src[i].ck << "/" << *src[i].p_ct << " "
		    << ar[src[i].id / 9][src[i].id % 9] << ": ";

		show_vect(src[i].avail, CT_D1);
	}

	cout << "--\n" << endl;
} //>

void SuDoKu::set_pos_state(unum seq, unum ck, unum i, unum j) //<1
{
	pos_state[seq].id = i * 9 + j;
	pos_state[seq].ck = ck;
	pos_state[seq].p_ar = &ar[i][j];
	pos_state[seq].p_ct = &ar_pos_ct[i][j];
	pos_state[seq].avail = ar_pos_avail[i][j];
}

unum SuDoKu::ar_commit_from_state(unum start, unum end) //<1
{
	/**
	 * commit num from pos_state to ar for unfilled pos if pos_ct==1
	 *
	 * @param the range to check for commit [start, end)
	 * @return how many pos committed
	 */
	unum ct_x = 0;

	for (unum i = start; i < end; i++)
		if (*pos_state[i].p_ar == 0 && *pos_state[i].p_ct == 1) {
			*pos_state[i].p_ar = pos_state[i].avail[0];
			ct_x++;
		}

	return ct_x;
} //>

unum SuDoKu::trim_for_related_rcg(unum i, unum j) //<1
{
	/**
	 * remove ar[i][j] from related ar_pos_avail only if the pos is unfilled
	 *
	 * keep ct and avail synced
	 *
	 * @param index of ar
	 * @return trimmed ct
	 */

	if (ar[i][j] == 0)
		return 0;

	unum g_id = idx_gn(i, j);		// group id
	unum g_seq = seq_gn(i, j);		// seq in the group of ar[i][j]

	// the rcg related element count for: (9-1) * 3 - 2 * 2 = 20
	unum * ptr_ct[20];				// the id of ptr_avail
	unum * ptr_avail[20];			// the avail list to be procceed

	unum ct_idx = 0, ct_trim = 0;

	// collect the avail to be procceed
	for (unum n = 0; n < 9; n++) {

		if ( n != j && ar[i][n] == 0 ) {	// row
			ptr_ct[ct_idx] = &ar_pos_ct[i][n];
			ptr_avail[ct_idx++] = ar_pos_avail[i][n];
		}

		if ( n != i && ar[n][j] == 0 ) {	// col
			ptr_ct[ct_idx] = &ar_pos_ct[n][j];
			ptr_avail[ct_idx++] = ar_pos_avail[n][j];
		}
	}

	// check row/col first, which helps avod dumplication by index
	for (unum n = 0; n < 9; n++) {

		unum n_i = idx_gi(g_id, n);
		unum n_j = idx_gj(g_id, n);

		if (n != g_seq && i != n_i && j != n_j && ar[n_i][n_j] == 0) {		// group
			ptr_ct[ct_idx] = &ar_pos_ct[n_i][n_j];
			ptr_avail[ct_idx++] = ar_pos_avail[n_i][n_j];
		}
	}

	// check before minus
	for (unum n = 0; n < ct_idx; n++)
		if ( *ptr_ct[n] == 1 && ptr_avail[n][0] == ar[i][j])
			return CT_ERR;

	// minus and sort
	for (unum n = 0; n < ct_idx; n++)
		if (minus_vect(ptr_avail[n], ar[i][j])) {

			// sort pos_avail and pos_ct
			sort_vect(ptr_avail[n], (*ptr_ct[n])--, SORT_FAST);
			ct_trim++;
		}

	return ct_trim;
}

bool SuDoKu::update_for_related_rcg(unum i, unum j) //<1
{
	/**
	 * update related update_pos_avail_and_ct only if the pos is unfilled
	 *
	 * @param index of ar
	 * @return removed ct
	 */

	unum g_id = idx_gn(i, j);		// group id
	unum g_seq = seq_gn(i, j);		// seq in the group of ar[i][j]

	unum idx[20];			// the id of selected pos
	unum ct_idx = 0;

	// collect the avail to be procceed
	for (unum n = 0; n < 9; n++) {

		if ( n != j && ar[i][n] == 0 )	// row
			idx[ct_idx++] = i * 9 + n;

		if ( n != i && ar[n][j] == 0 )	// col
			idx[ct_idx++] = n * 9 + j;
	}

	for (unum n = 0; n < 9; n++) {

		unum n_i = idx_gi(g_id, n);
		unum n_j = idx_gj(g_id, n);

		if (n != g_seq && i != n_i && j != n_j && ar[n_i][n_j] == 0)	// group
			idx[ct_idx++] = n_i * 9 + n_j;
	}

	// scan and update
	for (unum n = 0; n < ct_idx; n++)
		if (!update_pos_avail_and_ct_at_idx(idx[n] / 9, idx[n] % 9))
			return false;

	return true;
}

bool SuDoKu::update_pos_avail_and_ct_at_idx(unum i, unum j) //<1
{
	/**
	 * generate avail list and count pos_ct for ar[i][j]
	 *
	 * @return true if pos_ct > 0
	 */
	if (ar[i][j] > 0) {	// for filled pos, set directly

		memset(ar_pos_avail[i][j], 0, sizeof(ar_pos_avail[i][j]));
		ar_pos_avail[i][j][0] = ar[i][j];
		ar_pos_ct[i][j] = 1;

	} else {			// for unfilled pos, scan

		// init avail list for ar[i][j]
		for (unum n = 0; n < 9; n++)
			ar_pos_avail[i][j][n] = n + 1;

		unum g_id = idx_gn(i, j);	// group id
		unum g_seq = seq_gn(i, j);	// seq in the group of ar[i][j]

		// remove consumed num from avail list
		for (unum n = 0; n < 9; n++) {

			if ( n != j && ar[i][n] > 0 )	// row
				ar_pos_avail[i][j][ ar[i][n] - 1 ] = 0;

			if ( n != i && ar[n][j] > 0 )	// col
				ar_pos_avail[i][j][ ar[n][j] - 1 ] = 0;

			// no dumplicate check, as it's just 4 more rewrite of numbers

			unum n_i = idx_gi(g_id, n);
			unum n_j = idx_gj(g_id, n);

			if (n != g_seq && ar[n_i][n_j] > 0)	// group
				ar_pos_avail[i][j][ ar[n_i][n_j] - 1 ] = 0;
		}

		// sync ct and sort avail
		sort_vect(ar_pos_avail[i][j], CT_D1, SORT_FAST);
		ar_pos_ct[i][j] = len_vect(ar_pos_avail[i][j]);
	}

	if (ar_pos_ct[i][j] == 0)
		return false;
	else
		return true;
}

unum SuDoKu::sync_pos_avail_and_ct(unum seq) //<1
{
	/**
	 * sync pos_avail and pos_ct
	 *
	 * abort sync if get pos_ct==0 with seq>0
	 *
	 * @param if 0, init with LRUD seq, use seq of pos_state if nonzero
	 * @return counter for synced pos
	 */

	unum ct = 0, id;

	for (unum i = seq; i < CT_D2; i++) {
		if (seq == 0)
			id = i;
		else
			id = pos_state[i].id;

		if (update_pos_avail_and_ct_at_idx(id / 9, id % 9))
			ct++;
		else {
			// with seq==0, init mode, init all pos_ct and pos_avail
			if (seq == 0)
				ct += CT_ERR;
			else
				return CT_ERR;
		}
	}

	return ct;
}

unum SuDoKu::init_pos_state() //<1
{
	/**
	 * setup pos_state array, commit to ar if ct==1
	 *
	 * 1. sync ct and avail info from pos_ct and pos_avail
	 * 2. set pos_state.ck, and commit to ar if pos_ct == 1
	 * 3. place pos with ct=1 from the begining, and others after it
	 * 4. sort pos_state[] by pos_ct for pos_ct!=1
	 * 5. random reorder avail list if in RAND mode
	 *
	 * @return counter for avail list with length == 1
	 */
	unum ct_1 = 0, ct_x = 0;

	// place filled pos from the front, and unfilled from the behind
	for (unum i = 0; i < 9; i++)
		for (unum j = 0; j < 9; j++)
			if (ar_pos_ct[i][j] == 1) {
				set_pos_state(ct_1++, 0, i, j);

				if (ar[i][j] == 0)
					ar[i][j] = ar_pos_avail[i][j][0];
			} else
				set_pos_state(CT_D2 - ++ct_x, ar_pos_ct[i][j], i, j);

	// sort post_state by pos_ct to speadup solution
	for (unum i = ct_1; i < CT_D2; i++) {
		unum min = i;

		for (unum j = i + 1; j < CT_D2; j++)
			if (*pos_state[j].p_ct < *pos_state[min].p_ct)
				min = j;

		if (min != i)
			switch_ab(pos_state[i], pos_state[min]);
	}

	// SORT_FAST is applied once the avail list is generated
	if (mode.sort == SORT_RAND || mode.sort == SORT_SEQ)
		for (unum i = ct_1; i < CT_D2; i++)
			sort_vect( pos_state[i].avail, *pos_state[i].p_ct, mode.sort);

	return ct_1;
}
//>

bool SuDoKu::fill_check_for_brute(unum seq) //<1
{
	/**
	 * for MTHD_BRUTE
	 *	check for confliction in row/col/group
	 */
	unum idx_i = pos_state[seq].id / 9;
	unum idx_j = pos_state[seq].id % 9;
	unum num = pos_state[seq].avail[pos_state[seq].ck];

	unum g_id = idx_gn(idx_i, idx_j);	// group id of ar[idx_i][idx_j]
	unum g_seq = seq_gn(idx_i, idx_j);	// seq in group of ar[idx_i][idx_j]

	for (unum n = 0; n < 9; n++) {
		if (n != idx_j && ar[idx_i][n] == num)	// row
			return false;

		if (n != idx_i && ar[n][idx_j] == num)	// col
			return false;

		if (n != g_seq && ar[idx_gi(g_id, n)][idx_gj(g_id, n)] == num) // group
			return false;
	}

	return true;
}

bool SuDoKu::fill_check_for_calc(unum seq) //<1
{
	/**
	 * for MTHD_CALC
	 *	assume every candidate is valid to fill which eliminate the check in confliction
	 * check and trim in related row/col/group
	 */

	// prefill ar before trim_for_related_rcg
	*pos_state[seq].p_ar = pos_state[seq].avail[pos_state[seq].ck];
	unum ct_rm = trim_for_related_rcg(pos_state[seq].id / 9, pos_state[seq].id % 9);

	if (ct_rm < CT_ERR)
		return true;
	else {
		*pos_state[seq].p_ar = 0;		// reset the prefilled pos on fail
		return false;
	}
}

bool SuDoKu::check_and_fill(unum seq) //<1
{
	/**
	 * check and only commit to ar if check passed
	 */

	// the soduku puzzle may be invalid with some pos_ct == 0
	if (pos_state[seq].ck >= *pos_state[seq].p_ct)
		return false;

	// call fill check
	if ( (mode.mthd == MTHD_BRUTE && fill_check_for_brute(seq)) ||
	    (mode.mthd == MTHD_CALC && fill_check_for_calc(seq)) ) {

		// if check passed, fill ar with current ck
		*pos_state[seq].p_ar = pos_state[seq].avail[pos_state[seq].ck];
		return true;
	} else
		return false;
}

bool SuDoKu::traverse_state(unum seq_root) //<1
{
	/**
	 * traverse pos state tree from the given node
	 *
	 * itself does not change pos_state[].* other than .ck
	 *
	 * @param begin traverse with pos_state[seq_root]
	 * @return nothing
	 */

	// already answered from commit, prepare to return
	if (seq_root == CT_D2)
		save_sol();

	if ( DEBUG_LEVEL > DBGL_NONE )
		show_pos_state(pos_state, "init state:");

	for (unum i = seq_root; i < CT_D2;) {
		// only unfilled pos will have ck >= ct, sel the first
		if (pos_state[i].ck >= *pos_state[i].p_ct)
			pos_state[i].ck = 0;

		// fill ar with current seq of pos state and check
		if (check_and_fill(i)) {

			if (++i < CT_D2)
				continue;

			// if answered, do sth
			if (check_ar()) {

				save_sol();

				if ( DEBUG_LEVEL > DBGL_NONE )
					show_pos_state(pos_state, "succeed state:");

				if (mode.ans == ANS_FRST || sols.size() == MAX_SOLS_CT)
					break;
			}

			// reach to the end but failed in check_ar(), reset i to CT_D2-1 for the later
			// while() as every i-- need to be cleaned for MTHD_CALC
			i--;
		}

		// move backward: next answer or fallback
		// sel next avail num, and move backward
		while (++ pos_state[i].ck >= *pos_state[i].p_ct) {
			// exit after facllback beyond seq_root
			if ( i <= seq_root || i == 0)
				return true;

			// reset current failed fill before backward
			*pos_state[i--].p_ar = 0;

			// for calc, reset related rcg
			if (mode.mthd == MTHD_CALC) {
				//  after backward, reset related sub status: pos_avail, pos_ct, state ck
				*pos_state[i].p_ar = 0;		// reset current pos before update related_rcg
				update_for_related_rcg(pos_state[i].id / 9, pos_state[i].id % 9);
				reset_sub_state_ck(i + 1);	// reset sub state ck
			}
		}
	}

	return true;
}

bool SuDoKu::try_solve() //<1
{
	// check puzzle
	if (!check_ar(true)) {
		cout << "error: invalid puzzle!" << endl;
		return false;
	}

	// get current cticks
	clock_t cticks = clock();

	// init pos_ct and pos_avail
	sync_pos_avail_and_ct();
	// traverse from first position with len > 1
	traverse_state(init_pos_state());

	// count passed cticks
	cticks = clock() - cticks;

	// display solutions
	if (sols.size() > 0)
		show_sol();
	else {
		cout << "failed to solve!\n";
		ar_commit_from_state();
		show_ar_x(ar, "sudoku (committed):");
		show_ar_x(ar_pos_ct, "ar_pos_ct:");
	}

	// show the time
	cout << "time takes: " << setprecision(8) << fixed <<
	    static_cast<double>(cticks * 1000) / CLOCKS_PER_SEC << " ms\n" << endl;

	return (sols.size() > 0);
} //>

bool parse_args(int argc, char * argv[])  //<1
{
	/**
	 * parse args and set global variable:
	 *  puzzle_mode
	 *  puzzle_init
	 *  puzzle_num
	 */

	// parse args
	try {
		if (argc != 3)
			throw "invalid options";

		// parse [MODE]
		string str_mode = argv[1];

		puzzle_mode = {
			map_mthd.at(argv[1][0]),
			map_sort.at(argv[1][1]),
			map_ans.at(argv[1][2])
		};

		// parse [INIT]
		string str_init = argv[2];
		short pos = str_init.find(':');

		if (pos >= 0) {
			puzzle_init = str_init.substr(0, pos);
			puzzle_num = stoi(str_init.substr(pos + 1));
		} else {
			if ( str_init == "auto" || str_init == "fixed" )
				puzzle_init = str_init;
			else
				throw "invalid init mode";
		}

		// for fixed puzzle, the seq must be within the predefined array
		if (!( (puzzle_init == "fixed" && puzzle_num < puzzles_ct) ||
		        (puzzle_init == "auto" && puzzle_num < CT_D2) )) {
			cerr << "err: index of fixed puzzle if beyond the edge" << endl;
			return false;
		}

	} catch (...) {
		cout << "opts: [MODE] [INIT]\n"
		    << "\t[MODE]: solve mode combination of [CB][FSR][FA]\n"
		    << "\t[INIT]: the way to init sudoku puzzle(auto:num|fixed:seq)"
		    << endl;

		return false;
	}

	return true;
}

int main(int argc, char * argv[]) //<1
{
	if ( parse_args(argc, argv) ) {

		// init sudoku
		SuDoKu x(puzzle_init, puzzle_num, puzzle_mode);

		// display and try to solve
		x.show_ar();
		x.try_solve();
	}

	return 0;
} //>

/* vi: set ts=4 nowrap : */
