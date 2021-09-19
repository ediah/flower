vpath %.cpp ${wildcard ./src/*}

RELEASE=YES
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
	OPTIFLAGS= -O2
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
OBJ = ${SRC:.cpp=.o}

default:
	@mkdir -p bin
	@make mlc -j4
	@make mlc-test

mlc: $(OBJ)
	$(CC) ${addprefix ./bin/,${notdir ${OBJ}}} -o $@

mlc-test: ./test/test.cpp
	$(CC) $(CFLAGS) $< -o $@

check:
	@cppcheck ${CHFLAGS} ${addprefix ./src/,${SRC}} | grep %
	@cat ${REPORT} | column -t -s '|'

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o ./bin/${notdir $@}

.PHONY: clean

clean:
	rm -rf ./bin/* mlc mlc-test
