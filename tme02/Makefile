CC  = gcc -Wall -Wextra -Wfloat-equal -Wshadow -Wunreachable-code -Werror -pedantic -std=c99 -O2
BIN = bin
INC = include
LIB = lib
OBJ = obj
SRC = src
AR  = ar

all: $(OBJ) $(BIN) nthreads mutex sync detach barrier

$(OBJ):
	mkdir $@

$(BIN):
	mkdir $@

${BIN}/nthreads: ${SRC}/nthreads.c
	${CC} -o $@ $^ -I${INC} -lpthread

${BIN}/mutex: ${SRC}/mutex.c
	${CC} -o $@ $^ -I${INC} -lpthread

${BIN}/sync: ${SRC}/sync.c
	${CC} -o $@ $^ -I${INC} -lpthread

${BIN}/detach: ${SRC}/detach.c
	${CC} -o $@ $^ -I${INC} -lpthread

${BIN}/barrier: ${SRC}/barrier.c
	${CC} -o $@ $^ -I${INC} -lpthread

nthreads: ${BIN}/nthreads
	@echo "Compiling nthreads"

mutex: ${BIN}/mutex
	@echo "Compiling mutex"

sync: ${BIN}/sync
	@echo "Compiling sync"

detach: ${BIN}/detach
	@echo "Compiling detach"

barrier: ${BIN}/barrier
	@echo "Compiling barrier"

clean:
	rm -f ${OBJ}/* ${BIN}/* ${LIB}/*
