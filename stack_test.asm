section .data
    test_data   dq  12.344,
                dq  45.85,
                dq  3.1415,
                dq  2.7123

section .text

global _start
_start:
    push rbp
    mov rbp, rsp

    movsd xmm1, [test_data]
    movsd xmm0, xmm1
    vpermpd ymm0, ymm0, 0x39
    movsd xmm1, [test_data+8]
    movsd xmm0, xmm1
    vpermpd ymm0, ymm0, 0x39
    movsd xmm1, [test_data+16]
    movsd xmm0, xmm1
    vpermpd ymm0, ymm0, 0x39
    movsd xmm1, [test_data+24]
    movsd xmm0, xmm1
    vpermpd ymm0, ymm0, 0x39


    sub rsp, 0x20
    vmovdqu [rbp-0x20], ymm0 
    vpxor ymm0, ymm0, ymm0
    vmovdqu ymm0, [rbp-0x20] 

    mov rsp, rbp
    pop rbp
    mov rax, 0x60
    mov rdi, 0
    syscall
    
