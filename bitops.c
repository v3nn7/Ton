#include "bitops.h"

int ton_bit_and(int a, int b) { return a & b; }
int ton_bit_or(int a, int b) { return a | b; }
int ton_bit_xor(int a, int b) { return a ^ b; }
int ton_bit_not(int a) { return ~a; }
int ton_shift_left(int a, int n) { return a << n; }
int ton_shift_right(int a, int n) { return a >> n; }