vpath %.cpp ./src
vpath %.hpp ./src/inc

ALL=YES
COMPACT=YES
REPORT=./cppcheck/cppcheck.report

ifeq (${COMPACT},YES)
	CHTEMP={file}:{line}| {message} | [CWE:{cwe}]
else
	CHTEMP=CWE{cwe}: {message}\n{callstack}\n{code}
endif

ifeq (${ALL},YES)
	ENABLE= --enable=all --inconclusive
else
	ENABLE= --enable=warning
endif

CC= g++
CFLAGS = --std=c++11 -g -O0 -Wno-write-strings -DDEBUG
CHFLAGS=-I./src/inc --language=c++ -j4 -l4 --max-ctu-depth=20 --std=c++11 \
        --template='${CHTEMP}' --cppcheck-build-dir=./cppcheck ${ENABLE} \
		--output-file=${REPORT}

VPATH = ./src ./src/inc ./bin
SRC = ${shell ls ${VPATH} | grep \\.cpp}
OBJ = ${SRC:.cpp=.o}
DEP = ${shell ls ${VPATH} | grep \\.hpp}

default: mlc

mlc: $(OBJ)
	$(CC) ${addprefix ./bin/,${OBJ}} -o $@

check:
	@cppcheck ${CHFLAGS} ${addprefix ./src/,${SRC}} | grep %
	@cat ${REPORT} | column -t -s '|'

%.o: %.cpp
	$(CC) $(CFLAGS) -I ./src/inc -c $< -o ./bin/$@

.PHONY: clean

clean:
	rm -rf ./bin/*
