section .data
    dict            db  0x43,0x63,0x44,0x64,0x45,0x46,0x66,0x47,0x67,0x41,0x61,0x42
    pi              dq  3.14159265358979323846
    n_notes         dq  12.0
    base            dq  2.0
    freq            dq  261.63
    samplerate      dq  44100.0
    amplitude       dq  4096.0
    ymm_size        dq  4.0
    double_consts   dq  1.0,2.0,3.0,4.0

section .text

global notes_2_freq
global get_time_vector
global get_waves
global convolve
extern pow
extern sin

notes_2_freq:
    push rbp
    mov rbp, rsp
    sub rsp, 0x50
    xor rcx,rcx
    mov [rbp-0x38], rdi
    mov [rbp-0x40], rsi
    mov [rbp-0x44], edx

.main_loop:
    mov rsi, [rbp-0x40]
    vmovdqa xmm0, [rsi+rcx]                     ; xmm0 = buffer[0:15b]
    vpxor xmm5, xmm5, xmm5
    vpxor xmm6, xmm6, xmm6
    xor rdx, rdx

    .get_indexes:
        vpbroadcastb xmm1, byte [dict + rdx]    ;xmm1[0:15b] = dict[rdx]
        vmovd xmm2, edx                         
        vpbroadcastb xmm2, xmm2                 ;xmm2[0:15b] = edx (index)
        vpcmpeqb xmm3, xmm0, xmm1               ;xmm1[0:15b] == xmm0[0:15b] : xmm3 mask
        vpaddb xmm6, xmm6, xmm3                 ;save mask for later
        vpand xmm4, xmm3, xmm2                  
        vpaddb xmm5, xmm5, xmm4                 ;xmm5[0:15b] = indexes

        inc rdx
        cmp rdx, 12
        jb .get_indexes
                                                ;save in stack because call to get_frequency
    vmovdqu [rbp-0x10], xmm6                    ;store mask
    vmovdqu [rbp-0x20], xmm5                    ;store indexes
    mov [rbp - 0x28], rcx                       ;store counter
    xor rdx, rdx
    mov [rbp - 0x30], rdx                       ;store counter

    .get_frequencies:
    call get_frequency
    vmovdqu xmm6, [rbp-0x10]
    vmovdqu xmm5, [rbp-0x20]
    vpmovsxbq ymm7, xmm6                        ;ymm6[0:3q] = xmm6[0:3b]
    vpand ymm0, ymm0, ymm7                      ;ymm0[x] = 0 if not a note in mask
    
    mov rdi, [rbp-0x38]
    vmovdqu [rdi], ymm0                         ;destination[0:3q] = ymm0[0:3q]
    add rdi, 32                                 ;destination += 32 bytes
    mov [rbp-0x38], rdi

    vpshufd xmm5, xmm5, 0x39                    ;shuffle xmm5 to get wanted scalar
    vpshufd xmm6, xmm6, 0x39                    ;shuffle xmm6 to get wanted mask
    vmovdqu [rbp-0x10], xmm6                    ;store mask after shuffle
    vmovdqu [rbp-0x20], xmm5                    ;store indexes after shuffle
    mov rdx, [rbp - 0x30]                       
    inc rdx
    mov [rbp - 0x30], rdx                       
    cmp rdx, 4
    jb .get_frequencies

    mov rcx, [rbp-0x28] 
    add rcx, 16
    mov eax, [rbp-0x44]
    cmp rcx, rax
    jb .main_loop

    leave
    ret

   
get_frequency:
    push rbp
    mov rbp, rsp
    sub rsp, 0x30
    
    vmovq xmm0, [n_notes]
    vpermpd ymm0, ymm0, 0x00                    ; ymm0[0:3q] = 12.0
    vpxor ymm3, ymm3
    vpxor ymm1, ymm1
    vmovd eax, xmm5
    vmovd xmm1, eax                             ; xmm1 = xmm5[0:3q]
    vpunpcklbw ymm1, ymm1, ymm3 
    vpunpcklwd ymm1, ymm1, ymm3
    vcvtdq2pd ymm1, xmm1                        ; ymm1[0:3q] = float(note_index) 
    vdivpd ymm1, ymm1, ymm0                     ; ymm1[0:3q] = ymm1[0:3q]/12
    
    xor rdx, rdx
.calc_pow:
    vmovdqu [rbp-0x20], ymm1
    mov [rbp - 0x28], rdx
    vmovq xmm0, [base]  
    call pow 
    vmovdqu ymm1, [rbp-0x20]
    mov rdx, [rbp - 0x28]  
    movsd xmm1, xmm0                            ; ymm1[0q] = pow(ymm1[0q])
    vpermpd ymm1, ymm1, 0x39                    ; ymm1[0q] = ymm1[1q], ymm1[1q] = ymm1[2q], ... , ymm1[3q] = ymm1[0q]
    inc rdx
    cmp rdx, 4
    jb .calc_pow

    vmovq xmm0, [freq]
    vpermpd ymm0, ymm0, 0x00                    ; ymm0[0:3q] = 261.63
    vmulpd ymm0, ymm0, ymm1                     ; ymm0[0:3q] = ymm1[0:3q] * 261.63

    xor rdx, rdx 
    mov rsp, rbp
    pop rbp
    ret

get_time_vector:
    push rbp
    mov rbp, rsp
    vpxor ymm1, ymm1, ymm1
    vpxor ymm3, ymm3, ymm3
    vmovsd xmm1, [samplerate]                   ; xmm1 = samplerate
                                                ; xmm0 = duration
    vmulsd  xmm1, xmm0, xmm1                    ; xmm1 = samplerate*duration
    vcvtsd2si rax, xmm1                         ; rax = (long)xmm1[0q]
    xor rcx, rcx
    add rcx, 4
    vbroadcastsd ymm2, [ymm_size]                ; ymm2[0:3q] = 4.0
    vmovsd xmm4, [double_consts]
    movsd xmm3, xmm4
    vpermpd ymm3, ymm3, 0x39                    ; ymm3[0q] = ymm3[1q], ymm3[1q] = ymm3[2q], ... , ymm3[3q] = ymm3[0q]
    vmovsd xmm4, [double_consts+8]
    movsd xmm3, xmm4
    vpermpd ymm3, ymm3, 0x39                    ; ymm3[0q] = ymm3[1q], ymm3[1q] = ymm3[2q], ... , ymm3[3q] = ymm3[0q]

    vdivsd xmm0, xmm0, xmm1                     ; xmm0 = duration/samplerate
    vbroadcastsd ymm0, xmm0                     ; ymm0[0:3q] = duration/samplerate

    vmovsd xmm4, [double_consts+16]
    movsd xmm3, xmm4
    vpermpd ymm3, ymm3, 0x39                    ; ymm3[0q] = ymm3[1q], ymm3[1q] = ymm3[2q], ... , ymm3[3q] = ymm3[0q]
    vmovsd xmm4, [double_consts+24]
    movsd xmm3, xmm4
    vpermpd ymm3, ymm3, 0x39                    ; ymm3[0q] = ymm3[1q], ymm3[1q] = ymm3[2q], ... , ymm3[3q] = ymm3[0q]
                                                ; ymm3 = {1.0,2.0,3.0,4.0}
    vbroadcastsd ymm4, [pi]
    vbroadcastsd ymm5, [double_consts+8]
    vmulpd ymm0, ymm0, ymm4
    vmulpd ymm0, ymm0, ymm5                     ; ymm0[0:3q] = duration/samplerate * pi * 2
    vpxor xmm4, xmm4, xmm4
    vmovsd [rdi], xmm4
    add rdi, 8
.main_loop:
    vmulpd ymm1, ymm0, ymm3                     ; ymm1[0:3q] = duration/samplerate * {1.0,2.0,3.0,4.0}
    vmovdqu [rdi], ymm1
    vaddpd ymm3, ymm3, ymm2

    add rcx, 4
    add rdi, 32
    cmp rcx, rax
    jb .main_loop


    mov rsp, rbp
    pop rbp
    ret

; extern void get_waves(double* dst, double* freqs, double* time_v, int size, double duration);
get_waves:
    push rbp
    mov rbp, rsp
    sub rsp, 0x80                               ; rbp-0x8   = rax
                                                ; rbp-0x10  = rbx
                                                ; rbp-0x18  = rcx
                                                ; rbp-0x20  = rdx
                                                ; rbp-0x28  = rdi
                                                ; rbp-0x30  = rsi
                                                ; rbp-0x38  = time_v
   
    vmovsd xmm1, [samplerate]                   ; xmm1 = samplerate
                                                ; xmm0 = duration
    vmulsd  xmm1, xmm0, xmm1                    ; xmm1 = samplerate*duration
    vcvtsd2si rax, xmm1                         ; rax = (long)xmm1[0q]
    mov [rbp-0x8], rax
    mov [rbp-0x18], rcx
    mov [rbp-0x28], rdi
    mov [rbp-0x30], rsi
    mov [rbp-0x38], rdx
    
    xor rbx, rbx
.notes_loop:
    mov [rbp-0x10], rbx

    mov rbx, [rbp-0x38]                         ; rbx = time_v
    xor rdx, rdx
    .inner_loop:
        mov rsi, [rbp-0x30]
        vbroadcastsd ymm0, [rsi]
        mov [rbp-0x20], rdx
        vmovupd ymm1, [rbx+rdx*8]               ; ymm1 = times*pi*2
        vmulpd ymm0, ymm1, ymm0                 ; ymm0 = times*pi*2*freq
        xor rax, rax
        .sine_loop:

            vmovdqu [rbp-0x60], ymm0
            mov [rbp - 0x40], rax
            call sin
            movsd xmm1, xmm0                    ; ymm1[0q] = sin(ymm0[0q])
            vmovdqu ymm0, [rbp-0x60]
            movsd xmm0, xmm1                    
            mov rax, [rbp - 0x40]  
            vpermpd ymm0, ymm0, 0x39                    

            inc rax
            cmp rax, 4
            jb .sine_loop

        vbroadcastsd ymm2, [amplitude]
        vmulpd ymm0, ymm0, ymm2                 ; ymm0 = amplitude * sin (2 * pi * times * freq)
        mov rdi, [rbp-0x28]
        vmovdqu [rdi], ymm0
        add rdi, 32
        mov [rbp-0x28], rdi

        mov rdx, [rbp-0x20]
        add rdx, 4
        mov rax, [rbp-0x8]
        cmp rdx, rax
        jbe .inner_loop
    
    mov rsi, [rbp-0x30]
    add rsi, 8
    mov [rbp-0x30], rsi

    mov rcx, [rbp-0x18]
    mov rbx, [rbp-0x10]
    inc rbx,
    cmp rbx, rcx 
    jb .notes_loop


    mov rsp, rbp
    pop rbp
    ret


; params:
;   rdi: result
;   rsi: filter
;   rdx: signal
;   ecx: len_result
;   r8d: len_filter
;   r9d: len_signal
convolve:
    push rbp
    mov rbp, rsp
    sub rsp, 0x10
    
    xor rbx, rbx
.main_loop:
    mov [rbp-0x10], rbx                          ; save counter i
    mov [rbp-0x8], rcx                          ; save len_result
   
    mov rcx, rbx
    mov rax, r8
    dec rax
    xor rbx, rbx
    cmp rcx, rax                                ; i >= len_filter - 1
    jb .start_zero
    add rbx, rcx
    sub rbx, rax                                ; ebx = i - (len_filter-1)
    .start_zero:                                ; ebx start counter
    mov rax, [rbp-0x10]                         ; rax = i
    vmovsd xmm0, [rdi+rax*8]                    ; xmm0 = y[i]
    cmp rcx, r9
    jb .inner_loop
    mov rcx, r9
    dec rcx
    .inner_loop:
        vmovsd xmm1, [rdx+rbx*8]                ; xmm1 = x[j]
        mov r10, rax
        sub r10, rbx
        vmovsd xmm2, [rsi+r10*8]               ; xmm2 = h[i-j]
        vmulsd xmm1, xmm1, xmm2                 ; xmm1 = h[i-j]*x[j]
        vaddsd xmm0, xmm1                       ; xmm0 += xmm1

        inc rbx
        cmp rbx, rcx
        jbe .inner_loop
    vmovsd [rdi+rax*8], xmm0                    ; y[i] = xmm0
    mov rbx, [rbp-0x10]
    mov rcx, [rbp-0x8]
    inc rbx
    cmp rbx, rcx
    jb .main_loop

    mov rax, rdi
    mov rsp, rbp
    pop rbp
    ret
