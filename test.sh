#!/bin/bash

SUDOKU="./sudoku"

sample_ct=5

for ((idx=0; idx < 8; idx++)) {
	for mode in BFA BRA CFA CRA; do
		echo -n "$SUDOKU $mode fixed:$idx - "
		((t_ms=0))

		for ((i=0; i< sample_ct; i++)) {
			t_ms=$(echo "$t_ms + $($SUDOKU $mode fixed:$idx | tail -2 | grep -o '[0-9.]*')"|bc -l)
		}

		echo "$(echo $t_ms / $sample_ct|bc -l) ms"
	done
}
