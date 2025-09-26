CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -lm

SRCS = ast.c lexer.c parser.c main.c environment.c token.c value.c io.c bitops.c memory.c array.c struct.c error.c module.c interpreter_core.c builtin.c builtin_tonlib.c collections.c sha256.c md5.c interpreter_stmt.c interpreter_expr.c
OBJS = $(SRCS:.c=.o)
TARGET = ton

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del /F $(OBJS) $(TARGET) 2>nul || (exit 0)