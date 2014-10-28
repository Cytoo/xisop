CC  = gcc -Wall -Wextra -Wfloat-equal -Wshadow -Wunreachable-code -Werror -pedantic -std=c99 -O2 -lpthread -lrt
BIN = bin
INC = include
LIB = lib
OBJ = obj
SRC = src
AR  = ar

all: $(OBJ) $(BIN) $(LIB) chat_server chat_client

$(OBJ):
	mkdir $@

$(BIN):
	mkdir $@

$(LIB):
	mkdir $@

${OBJ}/chat_server.o: ${SRC}/chat_server.c
	${CC} -c -o ${OBJ}/chat_server.o ${SRC}/chat_server.c -I${INC}

${OBJ}/chat_client.o: ${SRC}/chat_client.c
	${CC} -c -o ${OBJ}/chat_client.o ${SRC}/chat_client.c -I${INC}

${BIN}/chat_server: ${OBJ}/chat_server.o
	${CC} -o ${BIN}/chat_server ${OBJ}/chat_server.o

${BIN}/chat_client: ${OBJ}/chat_client.o
	${CC} -o ${BIN}/chat_client ${OBJ}/chat_client.o

chat_server: ${BIN}/chat_server
	@echo "Compiling chat_server"

chat_client: ${BIN}/chat_client
	@echo "Compiling chat_client"

clean:
	rm -rf ${OBJ} ${BIN} ${LIB}
