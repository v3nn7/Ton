section .text
    global rdtsc_wrapper
    global cpuid_wrapper

rdtsc_wrapper:
    ; Returns the 64-bit Time Stamp Counter in EDX:EAX
    rdtsc
    ret

cpuid_wrapper:
    ; Input: EAX (function leaf)
    ; Output: EAX, EBX, ECX, EDX
    ; Save non-volatile registers
    push rbx
    push rbp
    push rsi
    push rdi

    ; Move the input EAX to the actual EAX register
    mov eax, edi ; First argument (eax_in) is in RDI on x64 System V ABI

    cpuid

    ; Store results in memory pointed to by arguments
    ; Second argument (eax_out) is in RSI
    ; Third argument (ebx_out) is in RDX
    ; Fourth argument (ecx_out) is in RCX
    ; Fifth argument (edx_out) is in R8

    mov [rsi], eax
    mov [rdx], ebx
    mov [rcx], ecx
    mov [r8], edx

    ; Restore non-volatile registers
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    ret

popcnt_wrapper:
    ; Input: RCX (64-bit integer)
    ; Output: RAX (count of set bits)
    popcnt rax, rcx
    ret

bsf_wrapper:
    ; Input: RCX (64-bit integer)
    ; Output: RAX (index of first set bit, or -1 if no bits are set)
    ; Check if input is zero
    test rcx, rcx
    jz .no_set_bits
    bsf rax, rcx
    ret
.no_set_bits:
    mov rax, -1
    ret

bsr_wrapper:
    ; Input: RCX (64-bit integer)
    ; Output: RAX (index of first set bit from MSB, or -1 if no bits are set)
    ; Check if input is zero
    test rcx, rcx
    jz .no_set_bits_bsr
    bsr rax, rcx
    ret
.no_set_bits_bsr:
    mov rax, -1
    ret