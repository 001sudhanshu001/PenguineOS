; ORG 0x7c00
; BITS 16

; start:
;     mov ah, 0eh
;     mov al, 'A'
;     mov bx, 0
;     int 0x10 ; calling BIOS

;     jmp $ ; Jump at this instruction only

; times 510- ($ - $$) db 0
; dw 0xAA55 ; this will be 55AA due to little Endian



ORG 0x7c00
BITS 16

start:
    mov si, message  ; Load message address into SI
    call print_message
    jmp $ ; loop at this instruction only

print_message:
    mov bx, 0  ; Clear BX (not required, but kept for consistency)
    .loop:
        lodsb ; Load byte from (SI) into AL, increment SI so that it points to next character
        cmp al, 0 ; Compare AL with 0 (null terminator)
        je .done ; If null, exit function
        call print_char
        jmp .loop ; Repeat loop
    .done:   ; Creating sublabel
        ret

print_char:
    mov ah, 0eh ; Set AH = 0x0E (BIOS print function)
    int 0x10 ; ; Call BIOS interrupt to print character
    ret

message: db 'Hello PenguineOS!', 0
times 510- ($ - $$) db 0
dw 0xAA55 ; Boot sector magic number. This will be 55AA due to little Endian
