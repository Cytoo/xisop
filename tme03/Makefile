CC  = gcc -Wall -Wextra -Wfloat-equal -Wshadow -Wunreachable-code -Werror -pedantic -std=c99 -O2 -lpthread
BIN = bin
INC = include
LIB = lib
OBJ = obj
SRC = src
AR  = ar

all: $(OBJ) $(BIN) $(LIB) uppercase

$(OBJ):
	mkdir $@

$(BIN):
	mkdir $@

$(LIB):
	mkdir $@

${OBJ}/uppercase.o: ${SRC}/uppercase.c
	${CC} -c -o ${OBJ}/uppercase.o ${SRC}/uppercase.c -I${INC}

${BIN}/uppercase: ${OBJ}/uppercase.o
	${CC} -o ${BIN}/uppercase ${OBJ}/uppercase.o

uppercase: ${BIN}/uppercase
	@echo "Compiling main"

clean:
	rm -rf ${OBJ} ${BIN} ${LIB}
