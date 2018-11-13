#!/usr/bin/make -nf
# c/c++

tgt ?= sudoku

src := ${tgt}.cxx
obj := $(subst .cxx,.o,${src})
out := $(subst .cxx,,${src})

########################
# all source code
src_code := ${src}
src_code += $(wildcard src/*.c)
src_code += $(wildcard src/*.cc)
src_code += $(wildcard src/*.cpp)
src_code += $(wildcard src/*.cxx)
src_code += $(wildcard include/*.h)

# toolchains conf, note and more related
src_misc :=
src_misc += $(wildcard *.cmd)
src_misc += Makefile make.vim todo

# some conf var
astyle_conf := .astylerc
ag_ignore := .ignore
ycm_conf := .ycm_extra_conf.py

src_misc += ${astyle_conf} ${ag_ignore} ${ycm_conf}
apdx :=

########################
backup_dir := ../backup
backup_file := ${backup_dir}/${tgt}-$(shell date +%Y%m%d_%H%M%S).txz


########################
CXX := g++

# options may be passed
Ox ?= -g
FLAGS ?=

# override deal with commond-line variable
CXXFLAGS := -pipe -std=c++14 -Wall -Wextra ${Ox} ${FLAGS}


########################
# build
${out}: ${src_code}
	$(CXX) ${CXXFLAGS} -o $@  $^

force: ${src_code}
	@- touch ${src}
	@- make tgt=${tgt} Ox=${Ox} FLAGS=${FLAGS}


.PHONY: exec format force clean clobber backup
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

backup: ${src_code} ${src_misc}
	@ printf "\n"
	@ [ -d ${backup_dir} ] || mkdir -p ${backup_dir}
	@ tar --xz -vcf ${backup_file} $(sort $^ ${apdx})
	@ printf "\nsaved to ${backup_file}\n"


# vi: set ts=4 noexpandtab readonly :

