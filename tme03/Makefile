CC  = gcc -Wall -Wextra -Wfloat-equal -Wshadow -Wunreachable-code -Werror -pedantic -std=c99 -O2 -lpthread
BIN = bin
INC = include
LIB = lib
OBJ = obj
SRC = src
AR  = ar

all: $(OBJ) $(BIN) $(LIB) uppercase pool stack stack_multiple threadchain

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

${OBJ}/thread_stack.o: ${SRC}/thread_stack.c
	${CC} -c -o ${OBJ}/thread_stack.o ${SRC}/thread_stack.c -I${INC}

${OBJ}/stack_main.o: ${SRC}/stack_main.c ${INC}/thread_stack.h
	${CC} -c -o ${OBJ}/stack_main.o ${SRC}/stack_main.c -I${INC}

${OBJ}/stack_multiple.o: ${SRC}/stack_multiple.c ${INC}/thread_stack.h
	${CC} -c -o ${OBJ}/stack_multiple.o ${SRC}/stack_multiple.c -I${INC}

${OBJ}/threadchain.o: ${SRC}/threadchain.c
	${CC} -c -o ${OBJ}/threadchain.o ${SRC}/threadchain.c -I${INC}

${BIN}/uppercase: ${OBJ}/uppercase.o
	${CC} -o ${BIN}/uppercase ${OBJ}/uppercase.o

${BIN}/pool: ${OBJ}/pool.o
	${CC} -o ${BIN}/pool ${OBJ}/pool.o

${BIN}/stack: ${OBJ}/stack_main.o ${OBJ}/thread_stack.o
	${CC} -o ${BIN}/stack ${OBJ}/stack_main.o ${OBJ}/thread_stack.o

${BIN}/stack_multiple: ${OBJ}/stack_multiple.o ${OBJ}/thread_stack.o
	${CC} -o ${BIN}/stack_multiple ${OBJ}/stack_multiple.o ${OBJ}/thread_stack.o

${BIN}/threadchain: ${OBJ}/threadchain.o
	${CC} -o ${BIN}/threadchain ${OBJ}/threadchain.o

uppercase: ${BIN}/uppercase
	@echo "Compiling uppercase"

pool: ${BIN}/pool
	@echo "Compiling pool"

stack: ${BIN}/stack
	@echo "Compiling stack"

stack_multiple: ${BIN}/stack_multiple
	@echo "Compiling stack_multiple"

threadchain: ${BIN}/threadchain
	@echo "Compiling threadchain"

clean:
	rm -rf ${OBJ} ${BIN} ${LIB}
