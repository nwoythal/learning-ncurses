#using gcc compiler
CC = gcc
#add flags
# -g for gdb
# -lncurses to link ncurses
# -Wall for all warnings
CFLAGS = -g -lncurses -Wall -std=c99
TARGET = control

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(TARGET).c -o $(TARGET) $(CFLAGS)

clean:
	$(RM) $(TARGET)
