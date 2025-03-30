ORG 0
BITS 16

jmp 0x7c0:start

start:
    cli ; Clear Interrupts, as we are going to change Segment Registers

    mov ax, 0x7c0
    mov ds, ax
    mov es, ax
    mov ax, 0x00
    mov sp, 0x7c00

    sti ; Enables Interrupts

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
