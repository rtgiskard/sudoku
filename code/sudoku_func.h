#ifndef INCLUDE_SUDOKU_FUNC_H
#define INCLUDE_SUDOKU_FUNC_H

#include "sudoku.h"

template <typename T>
void show_vect(T vect, unum ct = CT_D1, unum seq = 0);
template <typename T>
unum len_vect(T vect, unum ct = CT_D1);
template <typename T>
unum minus_vect(T vect_a, unum num, unum ct = CT_D1);
template <typename T>
unum minus_vect(T vect_a, T vect_b, unum ct = CT_D1);
template <typename T>
void sort_vect(T vect, unum ct, Mode_Sort mode);
template <typename T>
unum itst_vect(T vect_a, T vect_b, T vect_c, T vect, unum ct = CT_D1);
template <typename T>
void switch_ab(T & a, T & b);

// template functions implementation
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

//<1 sort_vect: reorder non-zero value in 1-D array
template <typename T>
void sort_vect(T vect, unum ct, Mode_Sort mode)
{
	/**
	 * sort from (0, ct) in vect
	 */
	switch (mode) {
		case SORT_FAST:	//<2 only move 0 to the end
			for (unum i = 0, j; i < ct; i++) {
				if (vect[i] == 0)
					for (j = i + 1; j < ct; j++)
						if (vect[j] != 0) {
							switch_ab(vect[j], vect[i]);
							break;
						}
			}

			break;

		case SORT_SEQ:	//<2 move evrything larger or 0 to the end
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

		case SORT_RAND:	//<2 just random switch

			srand(DBG_RAND_SEED);

			for (unum i = 0; i < ct; i++)
				switch_ab(vect[rand() % ct], vect[rand() % ct]);

			break;
	} //>2
}

//<1 minus_vect: remove num from a vect
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
	/**
	 * save common num from a,b,c to vect
	 *
	 * @return counter for same num in the vect
	 */
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
//>

#endif
