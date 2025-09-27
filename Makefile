CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -lm
AS = nasm
ASFLAGS = -f win64

SRCS = $(filter-out lexer_test.c mem_test.c, $(wildcard *.c)) tonlib_low.c
ASM_SRCS = tonlib_low.asm
ASM_OBJS = $(ASM_SRCS:.asm=_asm.o)
OBJS = array.o ast.o bitops.o builtin.o builtin_crypto.o builtin_tonlib.o collections.o environment.o error.o interpreter_core.o interpreter_decl.o interpreter_expr.o interpreter_macro.o interpreter_stmt.o io.o lexer.o main.o md5.o memory.o module.o parser.o sha256.o struct.o token.o tonlib_low.o value.o tonlib_low_asm.o
TARGET = ton.exe

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%_asm.o: %.asm
	$(AS) $(ASFLAGS) $< -o $@

clean:
	del /F $(OBJS) $(TARGET) 2>nul || (exit 0)