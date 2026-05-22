CC = gcc
CFLAGS = -Wall -g -Iinclude
SRC = src/main.c src/game.c src/storage.c
OBJ = $(SRC:.c=.o)
TARGET = minesweeper

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)
