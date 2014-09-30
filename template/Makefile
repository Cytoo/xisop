CC  = gcc -Wall -Wextra -Wfloat-equal -Wshadow -Wunreachable-code -Werror -pedantic -std=c99 -O2
BIN = bin
INC = include
LIB = lib
OBJ = obj
SRC = src
AR  = ar

all: $(OBJ) $(BIN) $(LIB) main

$(OBJ):
	mkdir $@

$(BIN):
	mkdir $@

$(LIB):
	mkdir $@

${OBJ}/main.o: ${SRC}/main.c
	${CC} -c -o ${OBJ}/main.o ${SRC}/main.c -I${INC}

${BIN}/main: ${OBJ}/main.o
	${CC} -o ${BIN}/main ${OBJ}/main.o

main: ${BIN}/main
	@echo "Compiling main"

clean:
	rm -rf ${OBJ} ${BIN} ${LIB}
