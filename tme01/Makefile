CC  = gcc -Wall -Wextra -Wfloat-equal -Wshadow -Wunreachable-code -Werror -pedantic -std=c99 -O2
BIN = bin
INC = include
LIB = lib
OBJ = obj
SRC = src
AR  = ar

all: $(OBJ) $(BIN) max_calcul nfork procstr sync procstr-no-wait procsig

$(OBJ):
	mkdir $@

$(BIN):
	mkdir $@

${OBJ}/max_func.o: ${SRC}/max_func.c
	${CC} -c -o ${OBJ}/max_func.o ${SRC}/max_func.c -I${INC}

${OBJ}/max_calcul.o: ${INC}/max_func.h ${SRC}/max_calcul.c
	${CC} -c -o ${OBJ}/max_calcul.o ${SRC}/max_calcul.c -I${INC}

${OBJ}/procstr.o: ${SRC}/procstr.c
	${CC} -c -o ${OBJ}/procstr.o ${SRC}/procstr.c -I${INC}

${OBJ}/procstr-no-wait.o: ${SRC}/procstr-no-wait.c
	${CC} -c -o ${OBJ}/procstr-no-wait.o ${SRC}/procstr-no-wait.c -I${INC}

${BIN}/max_calcul: ${OBJ}/max_calcul.o ${OBJ}/max_func.o
	${CC} -o ${BIN}/max_calcul ${OBJ}/max_calcul.o ${OBJ}/max_func.o

${BIN}/procstr: ${OBJ}/procstr.o
	${CC} -o ${BIN}/procstr ${OBJ}/procstr.o
	 
${BIN}/nfork: ${SRC}/nfork.c
	${CC} -o ${BIN}/nfork ${SRC}/nfork.c

${BIN}/sync: ${SRC}/sync.c
	${CC} -o ${BIN}/sync ${SRC}/sync.c

${BIN}/procsig: ${SRC}/procsig.c
	${CC} -o ${BIN}/procsig ${SRC}/procsig.c

${BIN}/procstr-no-wait: ${OBJ}/procstr-no-wait.o
	${CC} -o ${BIN}/procstr-no-wait ${OBJ}/procstr-no-wait.o
	 
max_calcul: ${BIN}/max_calcul
	@echo "Compiling max_calcul"

procstr: ${BIN}/procstr
	@echo "Compiling procstr"

nfork:	${BIN}/nfork
	@echo "Compiling nfork"

sync:	${BIN}/sync
	@echo "Compiling sync"

procstr-no-wait: ${BIN}/procstr-no-wait
	@echo "Compiling procstr-no-wait"

procsig:	${BIN}/procsig
	@echo "Compiling procsig"

clean:
	rm -f ${OBJ}/* ${BIN}/* ${LIB}/*
