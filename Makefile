vpath %.cpp ${wildcard ./src/*} ./src

RELEASE=NO
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
	@$(CC) ${addprefix ./bin/,${notdir ${OBJ}}} -o $@

mlc-test: ./test/test.cpp Makefile
	@echo "    CC    $@"
	@$(CC) $(CFLAGS) $< -o $@

check:
	@cppcheck ${CHFLAGS} ./src/ | grep %
	@cat ${REPORT} | column -t -s '|'

.PHONY: clean

clean:
	rm -rf ./bin/* mlc mlc-test
	rm -f ./compiled* ./optimized* out.bin
	rm -f Makefile.dep

%.o: %.cpp Makefile
	@echo "    CC    $@"
	@$(CC) $(CFLAGS) -c $< -o ./bin/${notdir $@}

include Makefile.dep

Makefile.dep: ./script/gendep.py
	@./script/gendep.py ./src ./test