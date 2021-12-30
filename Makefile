vpath %.cpp ${wildcard ./src/*} ./src

RELEASE=NO
COVERAGE=NO
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
	ENABLE= --enable=all --inconclusive --bug-hunting
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
SRC = ${shell ls ${VPATH} | grep \\.cpp}
OBJ = ${notdir ${SRC:.cpp=.o}}

default:
	@mkdir -p bin
	@make mlc mlc-test -j4 -s

mlc: $(OBJ) Makefile
	@echo "    LD    $@"
	@$(CC) $(LFLAGS) ${addprefix ./bin/,${notdir ${OBJ}}} -o $@

mlc-test: ./test/test.cpp Makefile
	@echo "    CC    $@"
	@$(CC) $(CFLAGS) $(LFLAGS) $< -o $@

check:
	@cppcheck ${CHFLAGS} ./src/ | grep %
	@cat ${REPORT} | column -t -s '|'

cov: mlc-test
	@rm -f *.gcno *.gcda *.info
	-@./mlc-test -r -O -c ./test/A-unit/*.ml ./test/B-unit/*.ml
	@lcov -c -d . -o ./coverage/mlc.info 2>/dev/null
	@lcov -o ./coverage/mlc-f.info --remove ./coverage/mlc.info \
					'/usr/include/*' \
					'$(shell pwd)/src/debugger/*' \
					'$(shell pwd)/test/*' 2>/dev/null | grep lines > ./coverage/lines.info
	@./script/updatecov.py ./coverage/lines.info

.PHONY: clean check cov

clean:
	rm -rf ./bin/* mlc mlc-test
	rm -f ./compiled* ./optimized* out.bin
	rm -f Makefile.dep
	rm -f *.gcno *.gcda *.info

%.o: %.cpp Makefile
	@echo "    CC    $@"
	@$(CC) $(CFLAGS) -c $< -o ./bin/${notdir $@}

include Makefile.dep

Makefile.dep: ./script/gendep.py
	@./script/gendep.py ./src ./test