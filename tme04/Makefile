CC  = gcc -Wall -Wextra -Wfloat-equal -Wshadow -Wunreachable-code -Werror -pedantic -std=c99 -O2
BIN = bin
INC = include
LIB = lib
OBJ = obj
SRC = src
AR  = ar

all: $(OBJ) $(BIN) $(LIB) fence stack multiqueue

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

${OBJ}/multiqueue.o: ${SRC}/multiqueue.c
	${CC} -c -o ${OBJ}/multiqueue.o ${SRC}/multiqueue.c -I${INC}

${BIN}/fence: ${OBJ}/fence.o
	${CC} -o ${BIN}/fence ${OBJ}/fence.o

${BIN}/stack: ${OBJ}/stack_multiple.o ${OBJ}/stack_implementation.o
	${CC} -o ${BIN}/stack ${OBJ}/stack_multiple.o ${OBJ}/stack_implementation.o

${BIN}/multiqueue: ${OBJ}/multiqueue.o
	${CC} -o ${BIN}/multiqueue ${OBJ}/multiqueue.o



fence: ${BIN}/fence
	@echo "Compiling fence"

stack: ${BIN}/stack
	@echo "Compiling multiple stack"

multiqueue: ${BIN}/multiqueue
	@echo "Compiling multiqueue"

clean:
	rm -rf ${OBJ} ${BIN} ${LIB}
