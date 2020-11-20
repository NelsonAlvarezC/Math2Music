section .data
    dict    db  0x43,0x63,0x44,0x64,0x45,0x46,0x66,0x47,0x67,0x41,0x61,0x42
    n_notes dd  0x0c
    base    dq  2.0

section .text

global math2music
extern pow

math2music:
    push rbp
    
    xor rcx,rcx
.main_loop:
    vmovdqa xmm0, [rdi+rcx]
    
    ;vpxor xmm3, xmm3, xmm3
    vpxor xmm5, xmm5, xmm5
    vpxor xmm6, xmm6, xmm6
    xor rdx, rdx
    .inner_loop:
        vpbroadcastb xmm1, byte [dict + rdx]
        vmovd xmm2, edx
        vpbroadcastb xmm2, xmm2

        vpcmpeqb xmm3, xmm0, xmm1
        vpaddb xmm6, xmm6, xmm3
        vpand xmm4, xmm3, xmm2
        vpaddb xmm5, xmm5, xmm4 

        inc rdx
        cmp rdx, 12
        jb .inner_loop
    call get_frequency
    
    add rcx, 16
    cmp rcx, rsi
    jb .main_loop

    leave
    ret

   
get_frequency:
    push rbp
    vmovd xmm0, [n_notes]
    vpbroadcastd xmm0, xmm0     ; xmm0[1:4] = 12
    vcvtdq2pd ymm0, xmm0        ; ymm0[1:4] = 12.0
    vpxor ymm3, ymm3
    vpxor ymm2, ymm2
    vmovd eax, xmm5
    vmovd xmm2, eax             ; xmm2 = xmm5[1:4]
    vpunpcklbw ymm2, ymm2, ymm3 
    vpunpcklwd ymm2, ymm2, ymm3
    vcvtdq2pd ymm2, xmm2        ; ymm2[1:4] = float(note_index) 

    xor rdx, rdx
.calc_pow:
    vmovq xmm0, [base]  
    vmovsd xmm1, xmm2
    call pow 
    movsd xmm2, xmm0
    vshufpd ymm2, ymm2, ymm2, 0x93
    inc rdx
    cmp rdx, 4
    jb .calc_pow


    xor rdx, rdx 

    leave
    ret
