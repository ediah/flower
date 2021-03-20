vpath %.cpp ./src
vpath %.hpp ./src/inc

CC= g++
CFLAGS = --std=c++11 -g -O0

VPATH = ./src ./src/inc ./bin
SRC = ${shell ls ${VPATH} | grep \\.cpp}
OBJ = ${SRC:.cpp=.o}
DEP = ${shell ls ${VPATH} | grep \\.hpp}

default: mlc

mlc: $(OBJ)
	$(CC) ${addprefix ./bin/,${OBJ}} -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) -I ./src/inc -c $< -o ./bin/$@
	
.PHONY: clean

clean:
	rm -rf ./bin/*
