#!/usr/bin/make -nf
# c/c++

tgt ?= sudoku

########################
# all source code
src_code_cc := $(wildcard code/*.cc)
src_code_h := $(wildcard code/*.h)
src_code_h += $(wildcard code/*.inl)
src_code := ${src_code_cc} ${src_code_h}

obj := $(subst .cc,.o,${src_code_cc})
out := ${tgt}

# some conf var
astyle_conf := .astylerc
ag_ignore := .ignore
ycm_conf := .ycm_extra_conf.py

########################
CXX := g++

# options may be passed
Ox ?= -g
FLAGS ?=

CXXFLAGS := -pipe -std=c++14 -Wall -Wextra ${Ox} ${FLAGS}

########################
# build
${out}: ${src_code}
	$(CXX) ${CXXFLAGS} -o $@  ${src_code_cc}

force: ${src_code}
	@- touch $<
	@- make tgt=${tgt} Ox=${Ox} FLAGS=${FLAGS}


.PHONY: exec format force clean clobber
########################
exec: ${out}
	@- ./${out}

format: ${src_code}
	@ astyle --mode=c --options=${astyle_conf} $(sort $^)

clean:
	@- rm -fv ${obj}
	@- rm -fv */*.astyle.ori
	@- rm -fv *.astyle.ori

clobber: clean
	@- rm -vf ${out}


# vi: set ts=4 noexpandtab readonly :
