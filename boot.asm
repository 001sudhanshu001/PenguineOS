ORG 0
BITS 16

_start:
    jmp short start

start:       

jmp 0x7c0:step2 ; This jumps to step2 while explicitly setting the Code Segment (CS) to 0x7C0.

; Interrupts :: https://www.ctyme.com/intr/rb-0607.htm

step2:
    cli ; Clear Interrupts, as we are going to change Segment Registers
    ;   Disabling Interrupts so that the segment setup doesn't get interrupted.

    mov ax, 0x7c0
    mov ds, ax ; Set Data Segment (DS) to 0x7C0 (so data accesses work correctly).
    mov es, ax
    mov ax, 0x00
    mov sp, 0x7c00

    sti ; Enables Interrupts

    ; https://www.ctyme.com/intr/rb-0607.htm
    mov ah, 2 ; BIOS READ Sector Command
    mov al, 1 ; Sector 1 sector(count of sectors to be read)
    mov ch, 0 ; Cylinder number (low eight bits)
    mov cl, 2 ; Sector number (sector 2) (because sector 1 is the boot sector).
    mov dh, 0 ; Head number
    mov bx, buffer ; Load the sector into the memory at label buffer.
    int 0x13 ;  BIOS interrupt to read sector

    jc error ; int 0x13 will set Carry flag if there is error while reading from disk

    mov si, buffer
    call print_message

    jmp $

    error:
        mov si, error_message  ; Load message address into SI
        call print_message
        jmp $

        
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
    int 0x10  ; Call BIOS interrupt to print character
    ret


error_message: db 'Failed to load sector', 0

times 510- ($ - $$) db 0
dw 0xAA55 ; Boot sector magic number. This will be 55AA due to little Endian


; This data is after first sector, so BIOS will not load this but we can reference it
; The data from 512 is referenced by this label and this will be read by 
; This is second sector after BL, so this will have the data of message.txt
buffer:     ; This will be used for storing the disk data in RAM