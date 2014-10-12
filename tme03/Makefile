CC  = gcc -Wall -Wextra -Wfloat-equal -Wshadow -Wunreachable-code -Werror -pedantic -std=c99 -O2 -lpthread
BIN = bin
INC = include
LIB = lib
OBJ = obj
SRC = src
AR  = ar

all: $(OBJ) $(BIN) $(LIB) uppercase pool

$(OBJ):
	mkdir $@

$(BIN):
	mkdir $@

$(LIB):
	mkdir $@

${OBJ}/uppercase.o: ${SRC}/uppercase.c
	${CC} -c -o ${OBJ}/uppercase.o ${SRC}/uppercase.c -I${INC}

${OBJ}/pool.o: ${SRC}/pool.c
	${CC} -c -o ${OBJ}/pool.o ${SRC}/pool.c -I${INC}

${BIN}/uppercase: ${OBJ}/uppercase.o
	${CC} -o ${BIN}/uppercase ${OBJ}/uppercase.o

${BIN}/pool: ${OBJ}/pool.o
	${CC} -o ${BIN}/pool ${OBJ}/pool.o

uppercase: ${BIN}/uppercase
	@echo "Compiling uppercase"

pool: ${BIN}/pool
	@echo "Compiling pool"

clean:
	rm -rf ${OBJ} ${BIN} ${LIB}
