CC  = gcc -Wall -Wextra -Wfloat-equal -Wshadow -Wunreachable-code -Werror -pedantic -std=c99 -O2
BIN = bin
INC = include
LIB = lib
OBJ = obj
SRC = src
AR  = ar

all: $(OBJ) $(BIN) $(LIB) fence

$(OBJ):
	mkdir $@

$(BIN):
	mkdir $@

$(LIB):
	mkdir $@

${OBJ}/fence.o: ${SRC}/fence.c
	${CC} -c -o ${OBJ}/fence.o ${SRC}/fence.c -I${INC}

${BIN}/fence: ${OBJ}/fence.o
	${CC} -o ${BIN}/fence ${OBJ}/fence.o

fence: ${BIN}/fence
	@echo "Compiling fence"

clean:
	rm -rf ${OBJ} ${BIN} ${LIB}
