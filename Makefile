vpath %.cpp ${wildcard ./src/*} ./src

RELEASE=YES
COVERAGE=YES
WITH_DRAWING=NO
ALL=YES
COMPACT=YES

REPORT=./cppcheck/report.log
SUPRLIST=./suprlist.txt

ifeq (${COMPACT},YES)
	CHTEMP={file}:{line}| {message} | [{id}]
else
	CHTEMP=CWE{cwe}: {message}\n{callstack}\n{code}
endif

ifeq (${ALL},YES)
	ENABLE= --enable=all --inconclusive
else
	ENABLE= --enable=warning
endif

ifeq (${RELEASE},YES)
	OPTIFLAGS= -O2 -g
else
	OPTIFLAGS= -O0 -g -DDEBUG
endif

ifeq (${COVERAGE},YES)
	OPTIFLAGS= -O0 -g --coverage
	LFLAGS=-lgcov --coverage
else
	LFLAGS=
endif

ifeq (${WITH_DRAWING},YES)
	OPTIFLAGS += -DDRAW_GRAPH
endif

CC= g++
CFLAGS = --std=c++11 -Wno-write-strings ${OPTIFLAGS} -I ./src
CHFLAGS=-I./src --language=c++ -j4 -l4 --max-ctu-depth=20 --std=c++11 \
        --template='${CHTEMP}' --cppcheck-build-dir=./cppcheck ${ENABLE} \
		--output-file=${REPORT} --suppressions-list=${SUPRLIST}

VPATH = ${wildcard ./src/*} ./bin

SRC_FLC   = ${shell ls ./src/common ./src/compiler ./src/optimizer | grep \\.cpp} ./src/flc.cpp
SRC_FLVM  = ${shell ls ./src/common ./src/runtime | grep \\.cpp} ./src/flvm.cpp
SRC_FLDBG = ${shell ls ./src/common ./src/runtime ./src/debugger | grep \\.cpp} ./src/fldbg.cpp
OBJ_FLC   = ${notdir ${SRC_FLC:.cpp=.o}}
OBJ_FLVM  = ${notdir ${SRC_FLVM:.cpp=.o}}
OBJ_FLDBG = ${notdir ${SRC_FLDBG:.cpp=.o}}

default:
	@mkdir -p bin
	@make flc flvm fldbg fltest -j4 -s

flc: $(OBJ_FLC) Makefile
	@echo "    LD    $@"
	@$(CC) $(LFLAGS) ${addprefix ./bin/,${notdir ${OBJ_FLC}}} -o $@

flvm: $(OBJ_FLVM) Makefile
	@echo "    LD    $@"
	@$(CC) $(LFLAGS) ${addprefix ./bin/,${notdir ${OBJ_FLVM}}} -o $@

fldbg: $(OBJ_FLDBG) Makefile
	@echo "    LD    $@"
	@$(CC) $(LFLAGS) ${addprefix ./bin/,${notdir ${OBJ_FLDBG}}} -o $@

fltest: ./test/test.cpp Makefile
	@mkdir -p bin
	@echo "    LD    $@"
	@$(CC) $(CFLAGS) $(LFLAGS) $< -o $@

check:
	@cppcheck ${CHFLAGS} ./src/
	@cat ${REPORT} | column -t -s '|'

report:
	@cat ${REPORT} | column -t -s '|'

cov: fltest
	@rm -f ./bin/*.gcno ./bin/*.gcda
	@rm -f *.gcno *.gcda *.info
	-@./fltest -r -O -c ./test/A-unit/*.fl ./test/B-unit/*.fl
	@mkdir -p ./coverage
	@lcov -c -d . -o ./coverage/flower.info 2>/dev/null
	@lcov -o ./coverage/flower-f.info --remove ./coverage/flower.info \
					'/usr/include/*' \
					'$(shell pwd)/test/*' 2>/dev/null | grep lines > ./coverage/lines.info
	@genhtml -o coverage ./coverage/flower-f.info
	@./script/updatecov.py ./coverage/lines.info

.PHONY: clean check cov report

clean:
	rm -rf ./bin/* flc fltest
	rm -f ./compiled* ./optimized* out.bin
	rm -f Makefile.dep
	rm -f *.gcno *.gcda *.info

clean-all: clean
	rm -rf ./cppcheck ./coverage ./doxygen/html ./doxygen/latex

%.o: %.cpp Makefile
	@echo "    CC    $@"
	@$(CC) $(CFLAGS) -c $< -o ./bin/${notdir $@}

include Makefile.dep

Makefile.dep: ./script/gendep.py
	@./script/gendep.py ./src ./test