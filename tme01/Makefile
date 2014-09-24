CC=gcc  -Wall -ansi -Wextra -Wfloat-equal -Wshadow -Wunreachable-code -Werror 
BIN=bin
INC=include
LIB=lib
OBJ=obj
SRC=src
AR=ar


all: max_calcul nfork procstr

${OBJ}/max_func.o: ${SRC}/max_func.c
	${CC} -c -o ${OBJ}/max_func.o ${SRC}/max_func.c -I${INC}

${OBJ}/max_calcul.o: ${INC}/max_func.h ${SRC}/max_calcul.c
	${CC} -c -o ${OBJ}/max_calcul.o ${SRC}/max_calcul.c -I${INC}

${OBJ}/procstr.o: ${SRC}/procstr.c
	${CC} -c -o ${OBJ}/procstr.o ${SRC}/procstr.c

${BIN}/max_calcul: ${OBJ}/max_calcul.o ${OBJ}/max_func.o
	${CC} -o ${BIN}/max_calcul ${OBJ}/max_calcul.o ${OBJ}/max_func.o

${BIN}/procstr: ${OBJ}/procstr.o
	${CC} -o ${BIN}/procstr ${OBJ}/procstr.o
	 
${BIN}/nfork: ${SRC}/nfork.c
	${CC} -o ${BIN}/nfork ${SRC}/nfork.c

max_calcul: ${BIN}/max_calcul
	echo "Compiling max_calcul"

procstr: ${BIN}/procstr
	echo "Compiling procstr"

nfork:	${BIN}/nfork
	echo "Compiling nfork"

clean:
	rm -f ${OBJ}/* ${BIN}/* ${LIB}/*
