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

${OBJ}/nthread.o: ${SRC}/nthread.c
	${CC} -c -o ${OBJ}/nthread.o ${SRC}/nthread.c -I${INC}

${BIN}/nthread: ${OBJ}/nthread.o
	${CC} -o ${BIN}/nthread ${OBJ}/nthread.o -lpthread

main: ${BIN}/nthread
	@echo "Compiling nthread"

clean:
	rm -rf ${OBJ} ${BIN} ${LIB}
