CC = gcc
CFLAGS = -Wall -g -Iinclude


SRC = src/main.c src/ejecutar.c src/path.c src/pipes.c src/redireccion.c src/procesar_comando.c src/job_list.c src/builtins.c src/historial_comandos.c src/signals.c src/flechas.c

OBJ = $(SRC:src/%.c=build/%.o)
BIN = bin/ucvsh

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJ)

build/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(BIN)
