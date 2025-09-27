CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -lm

SRCS = $(filter-out lexer_test.c mem_test.c, $(wildcard *.c))
OBJS = $(SRCS:.c=.o)
TARGET = ton.exe

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del /F $(OBJS) $(TARGET) 2>nul || (exit 0)