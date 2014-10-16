CC  = gcc -Wall -Wextra -Wfloat-equal -Wshadow -Wunreachable-code -Werror -pedantic -std=c99 -O2
BIN = bin
INC = include
LIB = lib
OBJ = obj
SRC = src
AR  = ar

all: $(OBJ) $(BIN) $(LIB) fence stack

$(OBJ):
	mkdir $@

$(BIN):
	mkdir $@

$(LIB):
	mkdir $@

${OBJ}/fence.o: ${SRC}/fence.c
	${CC} -c -o ${OBJ}/fence.o ${SRC}/fence.c -I${INC}

${OBJ}/stack_multiple.o: ${SRC}/stack_multiple.c
	${CC} -c -o ${OBJ}/stack_multiple.o ${SRC}/stack_multiple.c -I${INC}

${OBJ}/stack_implementation.o: ${SRC}/stack_implementation.c
	${CC} -c -o ${OBJ}/stack_implementation.o ${SRC}/stack_implementation.c -I${INC}

${BIN}/fence: ${OBJ}/fence.o
	${CC} -o ${BIN}/fence ${OBJ}/fence.o

${BIN}/stack: ${OBJ}/stack_multiple.o ${OBJ}/stack_implementation.o
	${CC} -o ${BIN}/stack ${OBJ}/stack_multiple.o ${OBJ}/stack_implementation.o

fence: ${BIN}/fence
	@echo "Compiling fence"

stack: ${BIN}/stack
	@echo "Compiling multiple stack"

clean:
	rm -rf ${OBJ} ${BIN} ${LIB}
