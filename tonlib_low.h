#ifndef TONLIB_LOW_H
#define TONLIB_LOW_H

#include <stdint.h>

// Function to read the Time Stamp Counter (TSC)
// Returns a 64-bit value representing the TSC.
extern uint64_t rdtsc_wrapper();

// Function to execute the CPUID instruction.
// eax_in: The value to be loaded into EAX before executing CPUID.
// eax_out, ebx_out, ecx_out, edx_out: Pointers to store the output values of EAX, EBX, ECX, EDX respectively.
extern void cpuid_wrapper(uint32_t eax_in, uint32_t *eax_out, uint32_t *ebx_out, uint32_t *ecx_out, uint32_t *edx_out);

// Function to count set bits (population count)
// Returns the number of set bits in a 64-bit value.
extern uint64_t popcnt_wrapper(uint64_t value);

// Function to find the index of the first set bit from LSB (Bit Scan Forward)
// Returns the index (0-63) or -1 if no bits are set.
extern int64_t bsf_wrapper(uint64_t value);

// Function to find the index of the first set bit from MSB (Bit Scan Reverse)
// Returns the index (0-63) or -1 if no bits are set.
extern int64_t bsr_wrapper(uint64_t value);

#endif // TONLIB_LOW_H