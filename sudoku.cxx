#include <iostream>
#include <iomanip>
#include <cmath>
#include <ctime>
#include <string>

using namespace std;

typedef unsigned short	unum;
typedef unsigned int	uint;
typedef unsigned long	ulong;

//<1 definition for compiler compatibility
#if defined(__clang__)
	#define FALLTHROUGH [[clang::fallthrough]]
#elif defined(__GNUC__)
	#define FALLTHROUGH __attribute__ ((fallthrough))
#endif
//>


#define SOLVE_MODE		SOLVE_MODE_CFA

#define DEBUG_LEVEL		DBGL_NONE
#define DEBUG_FB_CT_MAX	400000
#define DEBUG_FB_STEP	10000


//<1 debug level
#define DBGL_NONE		0
#define DBGL_INFO		1
#define DBGL_DETAIL		2
#define DBGL_TRACE		3
//>

//<1 methold to get mode part
#define MODE_MTHD(x)	(x & 0x0f00)
#define MODE_SORT(x)	(x & 0x00f0)
#define MODE_ANS(x)		(x & 0x000f)

//<1 solve mode switch detail
//#define MTHD_DEF				// entry: update_pos_state() -> fill_ar
//#define SORT_DEF				// entry: init_pos_state(), sort_vect
//#define ANS_DEF				// entry: traverse_choice() -> update_pos_state()

#define MTHD_CALC		0x0000	// solve by analyze
#define MTHD_BRUTE		0x0100	// solve by brute method

#define SORT_SEQ		0x0000	// force seq sort
#define SORT_RAND		0x0010	// force random sort
#define SORT_FAST		0x0020	// fast sort, only switch 0 with the last

#define ANS_FRST		0x0000	// fast, find the first answer
#define ANS_ALL			0x0001	// find all answer

//<1 solve mode combines
#define SOLVE_MODE_CSF	0x0000
#define SOLVE_MODE_CSA	0x0001
#define SOLVE_MODE_CRF	0x0010
#define SOLVE_MODE_CRA	0x0011
#define SOLVE_MODE_CFF	0x0020
#define SOLVE_MODE_CFA	0x0021
#define SOLVE_MODE_BSF	0x0100
#define SOLVE_MODE_BSA	0x0101
#define SOLVE_MODE_BRF	0x0110
#define SOLVE_MODE_BRA	0x0111
#define SOLVE_MODE_BFF	0x0120
#define SOLVE_MODE_BFA	0x0121
//>

//< misc define
#define CT_D1		9			// count for 1 dimension
#define CT_D2		(9*9)		// count for 2 dimension
#define CT_D3		(9*9*9)		// count for 3 dimension
#define CT_ERR		(CT_D3 + 1)	// if reached this count, considered ERR

#define UFIL		"."			// char to represent unfilled position

#define TGT_ALL		0			// sync_pos_avail init all pos_avail and pos_ct
#define TGT_UFIL	1			// sync_pos_avail update only unfilled pos
//>

typedef struct _Pos_Node {	//<1
	unum i;
	unum j;
	unum ct;
	unum ck;
	unum * avail;
} Pos_Node;
//>

//<1 function declaration
template <typename T>
void copy_ar_d2(T ar_dst, T ar_src, unum r = CT_D1, unum c = CT_D1);
template <typename T>
void copy_ar_d3(T ar_dst, T ar_src, unum r = CT_D1, unum c = CT_D1, unum d = CT_D1);
template <typename AR>
void init_ar_d2(AR ar, unum num = 0, unum r = CT_D1, unum c = CT_D1);
template <typename AR, typename T>
void init_ar_d2(AR ar, T vect_i, T vect_j, T vect_num, unum ct);
template <typename T>
void show_vect(T vect, unum ct = CT_D1, unum seq = 0);
template <typename T>
unum len_vect(T vect, unum ct = CT_D1);
template <typename T>
bool check_vect(T vect, unum ct = CT_D1);
template <typename T>
bool equal_vect(T vect_a, T vect_b, unum ct = CT_D1);
template <typename T>
unum minus_vect(T vect_a, unum num, unum ct = CT_D1);
template <typename T>
unum minus_vect(T vect_a, T vect_b, unum ct = CT_D1);
template <typename T>
void sort_vect(T vect, unum ct = CT_D1, unum mode = SOLVE_MODE);
template <typename T>
unum itst_vect(T vect_a, T vect_b, T vect_c, T vect, unum ct = CT_D1);
template <typename T>
void switch_ab(T & a, T & b);

void switch_pos_node(Pos_Node & A, Pos_Node & B);
void copy_pos_node(Pos_Node & src, Pos_Node & dst);
void show_pos_node(Pos_Node & src, unum seq = CT_D3);
void show_pos_node(Pos_Node & src, string prompt = "");
void show_pos_state(Pos_Node * src, string prompt = "");
//>

class SuDoKu {		//<1
  private:
	//<2
	unum		ar[9][9];				// array of numbers to fill
	unum		ar_pos_ct[9][9];		// array of the choice of every position
	unum		ar_pos_avail[9][9][9];	// array of avail num list of every position
	Pos_Node	pos_state[CT_D2];		// state of try
	//>2

  public:
	SuDoKu()		//<2
	{
		init_ar();
		// gen_ar(24);
	}

	SuDoKu(SuDoKu * src)		//<2
	{
		copy_ar_d2(ar, src -> ar);
		copy_ar_d2(ar_pos_ct, src -> ar_pos_ct);
		copy_ar_d3(ar_pos_avail, src -> ar_pos_avail);

		for (unum i = 0; i < CT_D2; i++)
			copy_pos_node(pos_state[i], src -> pos_state[i]);
	} //>2

	void init_ar()	//<2
	{
		// fill with 0 by default
		init_ar_d2(ar, 0);

		unum vect_i[] = {
			0, 0, 0, 0, 1, 2, 2, 2,
			3, 3, 3, 4, 4, 4, 5, 5,
			5, 6, 6, 6, 7, 8, 8, 8,
			8
		};
		unum vect_j[] = {
			3, 4, 6, 7, 3, 5, 6, 8,
			0, 1, 6, 0, 4, 8, 2, 7,
			8, 0, 2, 3, 5, 1, 2, 4,
			5
		};
		unum vect_num[] = {
			1, 8, 2, 5, 5, 2, 6, 3,
			2, 4, 3, 7, 9, 5, 8, 9,
			7, 6, 4, 9, 3, 3, 2, 5,
			7
		};

		init_ar_d2(ar, vect_i, vect_j, vect_num, 25);
	}

	bool gen_ar(unum ct = 20)	//<2
	{
		// fill with 0 by default
		init_ar_d2(ar, 0);

		// fill one pos random
		srand(time(NULL) + rand());
		ar[rand() % 9][rand() % 9] = rand() % 10;

		// generate a random sudoku answer via BRF mode
		unum mode = SOLVE_MODE_CRF;
		sync_pos_avail(TGT_ALL);
		traverse_choice(init_pos_state(mode), mode);

		// fill with 0
		unum ct_mask = 0;

		srand(time(NULL) + rand());

		// mask position random
		do {
			unum idx_i = rand() % 9;
			unum idx_j = rand() % 9;

			if (ar[idx_i][idx_j] != 0) {
				ar[idx_i][idx_j] = 0;
				ct_mask++;
			}
		} while (ct_mask < CT_D2 - ct);

		return true;
	}

	bool check_ar()	//<2
	{
		for (unum n = 0; n < 9; n++) {
			for (unum i = 0; i < 9; i++) {
				if (ar[n][i] == 0)
					return false;

				for (unum k = i + 1; k < 9; k++)
					if (ar[n][i] == ar[n][k])
						return false;

				for (unum k = i + 1; k < 9; k++)
					if (ar[i][n] == ar[k][n])
						return false;

				unum idx_ii = n / 3 * 3 + i / 3;
				unum idx_ij = n % 3 * 3 + i % 3;

				for (unum k = i + 1; k < 9; k++) {
					unum idx_ki = n / 3 * 3 + k / 3;
					unum idx_kj = n % 3 * 3 + k % 3;

					if (ar[idx_ii][idx_ij] == ar[idx_ki][idx_kj])
						return false;
				}
			}
		}

		return true;
	} //>2

	void show_ar()	//<2
	{
		show_ar_x(ar, "sudoku:");
	}

	void show_ar_x(unum ar_x[9][9], string prompt = "")	//<2
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
	} //>2

	void inverse_array(unum ar_x[9][9])	//<2
	{
		unum num_avail[9];

		for (unum i = 0; i < 9; i++) {

			// init avail list with 1-9 with sequence
			for (unum j = 0; j < 9; j++)
				num_avail[j] = j + 1;

			// del exist num from avail list
			for (unum j = 0; j < 9; j++)
				if (ar_x[i][j] != 0)
					num_avail[ar_x[i][j] - 1] = 0;

			unum ct = 0;

			// save avail list back to ar_x
			for (unum j = 0; j < 9; j++)
				if (num_avail[j] != 0)
					ar_x[i][ct++] = num_avail[j];

			// fill with 0 for unavailable numbers
			for (; ct < 9; ct++)
				ar_x[i][ct] = 0;
		}
	}

	//<2 trim_avail_group(T group, unum ct)
	template <typename T>
	unum trim_avail_group(T group, unum ct = CT_D1)
	{
		unum len[9];		// hold elem length
		unum len_g[9];		// len for p_group
		unum len_g_ct[9];	// len_ct for p_group
		unum * p_group[9];	// hold same length in seq

		unum ct_pg = 0;		// counter for p_group element
		unum ct_pg_m = 0;	// counter for p_group element minussed
		unum ct_x = 0;		// counter for trimmed position

		// get length and sort group by length in increase order
		for (unum i = 0; i < ct; i++) {
			len[i] = len_vect(group[i]);
			unum tmp, * p_tmp;

			for (unum j = 0; j < i; j++)		// check and resort
				if (len[i] < len[j]) {
					tmp = len[i];
					p_tmp = group[i];

					for (unum jj = i; jj > j; jj--) {
						len[jj] = len[jj - 1];
						group[jj] = group[jj - 1];
					}

					len[j] = tmp;
					group[j] = p_tmp;
				}
		}

		// put the avail with same length and those cout big enough together
		for (unum i = 0, ct_i = 1; i < ct - 1; i++) {
			if (len[i] == len[i + 1])
				ct_i++;
			else {
				if (len[i] <= ct_i)			// if dimension <= counter, may got one to minus
					for (unum j = i - ct_i + 1; j <= i; j++) {
						len_g[ct_pg] = len[j];
						len_g_ct[ct_pg] = ct_i;
						p_group[ct_pg++] = group[j];
					}

				ct_i = 1;
			}
		}

		// check and minus equal in p_group one by one
		for (unum i = 0, step; i < ct_pg; i += step) {

			step = len_g_ct[i];

			// then find the same and remove elements
			for (unum j = 0; j < step; j++) {
				unum eq_ct = 1;

				// how many equal to current
				for (unum k = 0; k < step; k++) {
					if (k == j)
						continue;

					if (equal_vect(p_group[i + k], p_group[i + j]))
						eq_ct++;
				}

				if (eq_ct == len_g[i]) {						// got the one to minus
					for (unum k = 0; k < ct; k++) {				// scan to minus one by one
						if ( group[k] == p_group[i + j] ||			// ignore same,
						    equal_vect(group[k], p_group[i + j]))	//+ ignore equal
							continue;

						unum ct_mx = minus_vect(group[k], p_group[i + j]);

						if (ct_mx > 0) {							// count real minus
							if (len[k] > ct_mx) {
								sort_vect(group[k], len[k], SORT_FAST);
								ct_x++;
							} else
								return CT_ERR;

							for (unum kk = i + step; kk < ct_pg; kk++)	// chain to (ct_pg_m >0)
								if (group[k] == p_group[kk]) {
									ct_pg_m++;
									break;
								}
						}
					}

					if (ct_pg_m > 0)						// if p_group element is changed
						return ct_x;

					// break to assume only one eq in step, and as the sequence is random,
					// this should better be the default choice
					break;
				} else if (eq_ct > len[i])					// conflict! case: a trimmed compared again
					return CT_ERR;
			}
		}

		return ct_x;
	}

	void set_pos_state(unum seq, unum ck, unum i, unum j)	//<2
	{
		pos_state[seq].i = i;
		pos_state[seq].j = j;
		pos_state[seq].ck = ck;
		pos_state[seq].ct = ar_pos_ct[i][j];
		pos_state[seq].avail = ar_pos_avail[i][j];
	}

	bool fill_ar_with_num(unum idx_i, unum idx_j, unum num, unum mode = SOLVE_MODE)	//<2
	{
		if (MODE_MTHD(mode) == MTHD_BRUTE) {
			unum idx_g	= idx_i / 3 * 3 + idx_j / 3;		// group id of idx_i and idx_j

			for (unum n = 0; n < 9; n++) {
				unum idx_g_n = idx_i % 3 * 3 + idx_j % 3;	// seq in group of idx_i and idx_j
				unum idx_g_i = idx_g / 3 * 3 + n / 3;		// idx_i of seq n in group idx_g
				unum idx_g_j = idx_g % 3 * 3 + n % 3;		// idx_j of seq n in group idx_g

				if (n != idx_j)		// row
					if (ar[idx_i][n] == num)
						return false;

				if (n != idx_i)		// col
					if (ar[n][idx_j] == num)
						return false;

				if (n != idx_g_n)	// group
					if (ar[idx_g_i][idx_g_j] == num)
						return false;
			}
		}

		ar[idx_i][idx_j] = num;

		return true;
	}

	bool fill_ar_from_stat(unum seq, unum mode = SOLVE_MODE)	//<2
	{
		if (pos_state[seq].ck < pos_state[seq].ct)
			return fill_ar_with_num(
			        pos_state[seq].i,
			        pos_state[seq].j,
			        pos_state[seq].avail[pos_state[seq].ck], mode);
		else
			return false;
	} //>2

	bool sync(unum target) //<2
	{
		do {
			if ( sync_pos_avail(target) >= CT_ERR ||
			    analyze_pos_avail() >= CT_ERR )
				return false;
		} while (ar_commit_with_ct() > 0);

		return true;
	}

	unum sync_pos_avail(unum target) //<2 return counter for filled pos
	{
		// scan to get the row, colume, group numbers
		unum row[9][9], col[9][9], group[9][9];

		init_ar_d2(row, 0);
		init_ar_d2(col, 0);
		init_ar_d2(group, 0);

		// initiate counter for group
		unum g_x, ct_x = 0;

		// check row, colume, group
		for (unum i = 0; i < 9; i++) {
			unum ct_r = 0, ct_c = 0, ct_g = 0;

			// get num
			for (unum j = 0; j < 9; j++) {
				if (ar[i][j] != 0)	// check row i
					row[i][ct_r++] = ar[i][j];

				if (ar[j][i] != 0)	// check colume i
					col[i][ct_c++] = ar[j][i];

				g_x = ar[i / 3 * 3 + j / 3][i % 3 * 3 + j % 3];

				if (g_x != 0)	// check colume i
					group[i][ct_g++] = g_x;
			}

			// there're chances some conflict ct==1 appear before commit
			// check vect for conflict
			if (!check_vect(row[i], ct_r) ||
			    !check_vect(col[i], ct_c) ||
			    !check_vect(group[i], ct_g) )
				return CT_ERR;
		}

		// generate avail list
		inverse_array(row);
		inverse_array(col);
		inverse_array(group);

		// generate avail array
		for (unum i = 0; i < 9; i++)
			for (unum j = 0; j < 9; j++)
				if (ar[i][j] == 0) {	// position to fill
					g_x = i / 3 * 3 + j / 3;
					ar_pos_ct[i][j] = itst_vect(row[i], col[j], group[g_x],
					        ar_pos_avail[i][j]);

					if (ar_pos_ct[i][j] == 0)
						return CT_ERR;
				} else {				// filled position
					ct_x++;

					if (target == TGT_ALL) {
						ar_pos_ct[i][j] = 1;
						ar_pos_avail[i][j][0] = ar[i][j];

						for (unum k = 1; k < 9; k++)
							ar_pos_avail[i][j][k] = 0;
					}
				}

		return ct_x;
	}

	unum analyze_pos_avail() //<2 return trimmed pos
	{
		// generate group to trim for col and group
		unum * p_group[9], ct_trim = 0;

		for (unum i = 0, ct_g; i < 9; i++) {
			// row
			ct_g = 0;

			for (unum j = 0; j < 9; j++)
				if (ar[i][j] == 0)
					p_group[ct_g++] = ar_pos_avail[i][j];

			if (ct_g > 2) {	// at least 2 equal + 1 extral
				ct_trim += trim_avail_group(p_group, ct_g);

				if (ct_trim >= CT_ERR)
					break;
			}

			// col
			ct_g = 0;

			for (unum j = 0; j < 9; j++)
				if (ar[j][i] == 0)
					p_group[ct_g++] = ar_pos_avail[j][i];

			if (ct_g > 2) {	// at least 2 equal + 1 extral
				ct_trim += trim_avail_group(p_group, ct_g);

				if (ct_trim >= CT_ERR)
					break;
			}

			// group
			ct_g = 0;

			for (unum j = 0; j < 9; j++) {
				unum idx_g_i = i / 3 * 3 + j / 3;
				unum idx_g_j = i % 3 * 3 + j % 3;

				if (ar[idx_g_i][idx_g_j] == 0)
					p_group[ct_g++] = ar_pos_avail[idx_g_i][idx_g_j];
			}

			if (ct_g > 2) {	// at least 2 equal + 1 extral
				ct_trim += trim_avail_group(p_group, ct_g);

				if (ct_trim >= CT_ERR)
					break;
			}
		}

		// for trimmed and failed trim, update pos_ct
		if (ct_trim > 0)
			sync_pos_ct();

		return ct_trim;
	}

	unum sync_pos_ct()	//<2 return counter for pos with len = 1
	{
		unum ct_x = 0;

		for (unum i = 0; i < 9; i++)
			for (unum j = 0; j < 9; j++) {
				ar_pos_ct[i][j] = len_vect(ar_pos_avail[i][j]);

				if (ar_pos_ct[i][j] == 1)
					ct_x++;
				else if (ar_pos_ct[i][j] == 0)
					return CT_ERR;
			}

		return ct_x;
	}

	unum ar_commit_with_ct()	//<2 commited pos
	{
		unum ct_x = 0;

		for (unum i = 0; i < 9; i++)
			for (unum j = 0; j < 9; j++)
				if (ar[i][j] == 0 && ar_pos_ct[i][j] == 1) {
					ar[i][j] = ar_pos_avail[i][j][0];
					ct_x++;
				}

		return ct_x;
	} //>2

	inline void reset_sub_state(unum seq_root)	//<2
	{
		for (unum i = seq_root; i < CT_D2; i++) {
			pos_state[i].ct = ar_pos_ct[pos_state[i].i][pos_state[i].j];
			pos_state[i].ck = pos_state[i].ct;
		}
	}

	inline void reset_sub_ar(unum seq_root)	//<2
	{
		for (unum i = seq_root; i < CT_D2; i++)
			ar[pos_state[i].i][pos_state[i].j] = 0;
	}

	unum init_pos_state(unum mode = SOLVE_MODE)	//<2 return counter for len = 1
	{
		unum ct_1 = 0, ct_x = 0;

		for (unum i = 0; i < 9; i++)
			for (unum j = 0; j < 9; j++)
				if (ar_pos_ct[i][j] == 1)
					set_pos_state(ct_1++, 0, i, j);
				else
					set_pos_state(CT_D2 - ++ct_x, ar_pos_ct[i][j], i, j);

		if (MODE_SORT(mode) == SORT_RAND) {
			ct_x = CT_D2 - ct_1;

#if ( DEBUG_LEVEL >= DBGL_DETAIL ) //<3
			show_pos_state(pos_state, "pos_state after init:");
#endif

#if ( DEBUG_LEVEL == DBGL_NONE ) //<3
			srand(time(NULL) + rand());
#endif //>3

			// if randomize the pos node sequence, it will be much harder to
			// try every possibility to get the solution (line by line brute is much faster)
			/*
				for (unum i = 0; i < CT_D2; i++) {
					switch_pos_node(
					    pos_state[rand() % ct_x + ct_1],
					    pos_state[rand() % ct_x + ct_1]);
					switch_pos_node(
					    pos_state[i % ct_x + ct_1],
					    pos_state[rand() % ct_x + ct_1]);
				}
			*/

#if ( DEBUG_LEVEL == DBGL_NONE ) //<3
			srand(time(NULL) + rand());
#endif //>3

			// avail list
			for (unum i = 0, idx; i < CT_D2; i++) {
				idx = rand() % ct_x + ct_1;
				sort_vect( pos_state[idx].avail, pos_state[idx].ct, SORT_RAND);
				idx = i % ct_x + ct_1;
				sort_vect( pos_state[idx].avail, pos_state[idx].ct, SORT_RAND);
			}

#if ( DEBUG_LEVEL >= DBGL_DETAIL ) //<3
			show_pos_state(pos_state, "pos_state after randomize:");
#endif //>3
		}

		return ct_1;
	}

	bool update_pos_state(unum seq, unum mode = SOLVE_MODE)	//<2
	{
		switch (MODE_MTHD(mode)) {
			case MTHD_BRUTE:
				return fill_ar_from_stat(seq, mode);

			case MTHD_CALC:
				if (!fill_ar_from_stat(seq, mode))
					return false;

				// leave current pos ck handled by traverse
				if (sync(TGT_UFIL)) {
					reset_sub_state(seq + 1);

					return true;
				} else {
					reset_sub_ar(seq + 1);

					return false;
				}

			default:
				return false;
		}
	} //>2

	ulong traverse_choice(unum seq_root, unum mode = SOLVE_MODE)	//<2
	{
		ulong ct_ans = 0;

#if ( DEBUG_LEVEL > DBGL_NONE ) //<3
#define SHOW_DBG_CT	\
	cout << "dbg_ct: fb psd/all: " << dbg_ct_fb << ' ' \
	    << dbg_ct_psd << '/' << dbg_ct_all << '\n'

#define SHOW_DBG_TRACE(x) \
	do { \
		for (unum iiii = seq_root; iiii <= x; iiii++) \
			cout << pos_state[iiii].avail[pos_state[iiii].ck]; \
		cout << setw(CT_D2 - x) << ' '; \
		\
		for (unum iiii = seq_root; iiii <= x; iiii++) \
			cout << pos_state[iiii].ct - pos_state[iiii].ck; \
		cout << '\n'; \
	} while (false)

		ulong dbg_ct_fb = 0;
		ulong dbg_ct_psd = 1;
		ulong dbg_ct_all = 1;

		for (unum i = seq_root; i < CT_D2; i++)
			dbg_ct_all *= pos_state[i].ct;

#endif //>3

		for (unum i = seq_root; i < CT_D2;) {
			// sel 0 of ck of unfilled pos
			if (pos_state[i].ct == pos_state[i].ck)
				pos_state[i].ck = 0;

			if (update_pos_state(i, mode)) {

				// if the next node is pre filled, it's filled with ar_commit
				// and is already considered when update pos info
				while (++i < CT_D2 && ar[pos_state[i].i][pos_state[i].j] != 0)
					pos_state[i].ck = 0;

				if (i < CT_D2)
					continue;

				if (check_ar())
					ct_ans++;
				else
					break;

				if (MODE_ANS(mode) == ANS_FRST)
					break;
				else {
#if ( DEBUG_LEVEL >= DBGL_INFO ) //<3
					SHOW_DBG_CT;
#endif
#if ( DEBUG_LEVEL >= DBGL_DETAIL )
					show_pos_state(pos_state, "succeed state:");
#endif //>3

					show_ar_x(ar, "solution[" + to_string(ct_ans) + "]:");

					i--;		// prepare continue on success
				}
			}

#if ( DEBUG_LEVEL >= DBGL_INFO ) //<3
			dbg_ct_fb++;

			if (dbg_ct_fb % DEBUG_FB_STEP == 0)
				SHOW_DBG_CT;

			if (DEBUG_FB_CT_MAX != 0 &&
			    DEBUG_FB_CT_MAX < dbg_ct_fb)
				break;

#if ( DEBUG_LEVEL >= DBGL_TRACE )
			SHOW_DBG_TRACE(i);
#endif
#endif //>3

			// maybe a little triky but better for code orgnization and efficient
			// if ck < ct, the pos will be filled with ck in next update_pos_state
			while (pos_state[i].ct == ++(pos_state[i].ck)) {

				ar[pos_state[i].i][pos_state[i].j] = 0;

				if (--i < seq_root) {
					i = CT_D2;
					break;			// if possible share a same exit point
				}
			}
		}

#if ( DEBUG_LEVEL >= DBGL_INFO ) //<3
		SHOW_DBG_CT;
#endif //>3
		return ct_ans;
	}

	bool try_solve(unum mode = SOLVE_MODE)	//<2
	{
		// sync avail info, init pos_state tree,
		if (!sync(TGT_ALL)) {
			show_ar_x(ar, "invalid sudoku which deduced to:");
			return false;
		}

		// get current cticks
		clock_t cticks = clock();

		// traverse from first position with len > 1
		unum seq_root = init_pos_state(mode);
		ulong ct_ans = traverse_choice(seq_root, mode);

		// count passed cticks
		cticks = clock() - cticks;

		switch (ct_ans) {
			case 0:
				cout << "failed to solve!\n";
				show_ar_x(ar, "sudoku:");
				ar_commit_with_ct();
				show_ar_x(ar, "sudoku (committed):");
				show_ar_x(ar_pos_ct, "ar_pos_ct:");
				break;

			case 1:
				if (MODE_ANS(mode) == ANS_FRST)
					show_ar_x(ar, "solution:");

				FALLTHROUGH;

			default:
				cout << "done!\n" << endl;
				break;
		}

		// show the time
		cout << "time takes: " <<
		    float(cticks) / CLOCKS_PER_SEC << " s\n" << endl;

		return (ct_ans > 0);
	} //>2
}; //>

//<1 copy_ar
template <typename T>
void copy_ar_d2(T ar_dst, T ar_src, unum r, unum c)
{
	for (unum i = 0; i < r; i++)
		for (unum j = 0; j < c; j++)
			ar_dst[i][j] = ar_src[i][j];
}

template <typename T>
void copy_ar_d3(T ar_dst, T ar_src, unum r, unum c, unum d)
{
	for (unum i = 0; i < r; i++)
		for (unum j = 0; j < c; j++)
			for (unum k = 0; k < d; k++)
				ar_dst[i][j][k] = ar_src[i][j][k];
}

//<1 init_ar
template <typename AR>
void init_ar_d2(AR ar, unum num, unum r, unum c)
{
	for (unum i = 0; i < r; i++)
		for (unum j = 0; j < c; j++)
			ar[i][j] = num;
}

template <typename AR, typename T>
void init_ar_d2(AR ar, T vect_i, T vect_j, T vect_num, unum ct)
{
	for (unum n = 0; n < ct; n++)
		ar[vect_i[n]][vect_j[n]] = vect_num[n];
}

//<1 show_vect
template <typename T>
void show_vect(T vect, unum ct, unum seq)
{
	if (seq != 0)
		cout << seq << ':';

	for (unum i = 0; i < ct; i++)
		if (vect[i] == 0)
			cout << right << setw(2) << UFIL;
		else
			cout << right << setw(2) << vect[i];

	cout << endl;
}

//<1 len_vect
template <typename T>
unum len_vect(T vect, unum ct)
{
	unum ct_x = 0;

	for (unum i = 0; i < ct; i++)
		if (vect[i] > 0)
			ct_x++;

	return ct_x;
}

//<1 sort_vect
template <typename T>
void sort_vect(T vect, unum ct, unum mode)
{
	switch (MODE_SORT(mode)) {
		case SORT_FAST:	//<2
			for (unum i = 0, j; i < ct;) {
				if (vect[i] != 0)
					i++;
				else {
					for (j = i + 1; j < ct; j++)
						if (vect[j] != 0) {
							switch_ab(vect[j], vect[i]);
							break;
						}

					i = j;
				}
			}

			break;

		case SORT_SEQ:	//<2
			for (unum i = 0; i < ct; i++) {
				unum min = i;

				for (unum j = i + 1; j < ct; j++) {
					if (vect[j] == 0)
						continue;

					if (vect[j] < vect[min] || vect[min] == 0)
						min = j;
				}

				if (min != i)
					switch_ab(vect[min], vect[i]);
			}

			break;

		case SORT_RAND:	//<2
			for (unum i = 0; i < ct; i++)
				switch_ab(vect[rand() % ct], vect[rand() % ct]);

			break;
	} //>2
}

//<1 equal_vect
template <typename T>
bool equal_vect(T vect_a, T vect_b, unum ct)
{
	unum len_a = len_vect(vect_a, ct);
	unum len_b = len_vect(vect_b, ct);

	if (len_a != len_b)
		return false;

	for (unum i = 0; i < len_a; i++)
		if (vect_a[i] != vect_b[i])
			return false;

	return true;
}

//<1 check_vect
template <typename T>
bool check_vect(T vect, unum ct)
{
	for (unum i = 0; i < ct; i++)
		if (vect[i] != 0)
			for (unum j = i + 1; j < ct; j++)
				if (vect[i] == vect[j])
					return false;

	return true;
}

//<1 minus_vect
template <typename T>
unum minus_vect(T vect_a, unum num, unum ct)
{
	for (unum i = 0; i < ct; i++)
		if (vect_a[i] == num) {
			vect_a[i] = 0;
			return 1;
		}

	return 0;
}

template <typename T>
unum minus_vect(T vect_a, T vect_b, unum ct)
{
	unum len_a = len_vect(vect_a, ct);
	unum len_b = len_vect(vect_b, ct);
	unum ct_x = 0;

	for (unum i = 0; i < len_b; i++)
		ct_x += minus_vect(vect_a, vect_b[i], len_a);

	return ct_x;
}

//<1 itst_vect
template <typename T>
unum itst_vect(T vect_a, T vect_b, T vect_c, T vect, unum ct)
{
	unum cm_ct = 0;

	// compare seq: vect_c -> vect_a -> vect_b
	for (unum i = 0; i < ct; i++) {
		if (vect_c[i] == 0)
			break;

		for (unum j = 0; j < ct; j++) {
			if (vect_a[j] == 0)
				break;

			if (vect_a[j] == vect_c[i])
				for (unum k = 0; k < ct; k++) {
					if (vect_b[k] == 0)
						break;

					if (vect_b[k] == vect_c[i])
						vect[cm_ct++] = vect_c[i];
				}
		}
	}

	for (unum i = cm_ct; i < ct; i++)
		vect[i] = 0;

	return cm_ct;
}

//<1 switch_ab
template <typename T>
void switch_ab(T & a, T & b)
{
	T tmp = a;
	a = b;
	b = tmp;
}

void switch_pos_node(Pos_Node & A, Pos_Node & B)	//<1
{
	if (&A != &B) {
		switch_ab(A.i, B.i);
		switch_ab(A.j, B.j);
		switch_ab(A.ct, B.ct);
		switch_ab(A.ck, B.ck);
		switch_ab(A.avail, B.avail);
	}
}

void copy_pos_node(Pos_Node & src, Pos_Node & dst)  	//<1
{
	dst.i = src.i;
	dst.j = src.j;
	dst.ct = src.ct;
	dst.ck = src.ck;
	dst.avail = src.avail;		// share the avail list
}

void show_pos_node(Pos_Node & src, unum seq)	//<1
{
	if (seq < CT_D3)
		cout << right << setw(4) << seq << " ";

	cout << "(" << src.i << "," << src.j << ")  "
	    << src.ck << "/" << src.ct << " ";
	show_vect(src.avail, CT_D1);
}

void show_pos_node(Pos_Node & src, string prompt)	//<1
{
	if (prompt.length() != 0)
		cout << prompt;

	cout << "(" << src.i << "," << src.j << ")  "
	    << src.ck << "/" << src.ct << " ";
	show_vect(src.avail, CT_D1);
}

void show_pos_state(Pos_Node * src, string prompt)	//<1
{
	if (prompt.length() != 0)
		cout << prompt << '\n';

	for (unum i = 0; i < CT_D2; i++)
		show_pos_node(src[i], i);

	cout << "--\n" << endl;
}
//>

//unum main(unum argc, char * argv[])
int main(void)   //<1
{
	SuDoKu x;

	x.show_ar();
	x.try_solve();

	return 0;
} //>

/* vi: set ts=4 nowrap : */
