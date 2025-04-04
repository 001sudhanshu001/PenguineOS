ORG 0x7C00
BITS 16

CODE_SEG equ gdt_code - gdt_start ; 0x8  ; gdt_code  represent the address of first instruction under it and same with others
DATA_SEG equ gdt_data - gdt_start ; 0x10 ; These offset are predecided and fixed

_start:
    jmp short start

start:       
    jmp 0:step2 ; This jumps to step2 while explicitly setting the Code Segment (CS) to 0.

; Interrupts :: https://www.ctyme.com/intr/rb-0607.htm

step2:
    ; Disabling Interrupts so that the segment setup doesn't get interrupted.
    cli ; Clear Interrupts, as we are going to change Segment Registers

    mov ax, 0x00
    mov ds, ax ; Update Data Segment Register
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    sti ; Start Interrupts


.load_protected:
    cli
    lgdt[gdt_descriptor]
    mov eax, cr0    ; Load CR0 into EAX
    or eax, 0x1     ; Set the PE (Protection Enable) bit
    mov cr0, eax    ; Write back to CR0 (Now in Protected Mode!)
    jmp CODE_SEG:load32 ; Update CS with the CODE_SEG for protected mode, and jump to label load32


; GDT -> It has the fixed default values.
; The order of gdt is fixed, for null, code & data. Also in these labels
gdt_start:

gdt_null:  ; 64 Bits of 0 (32 bits each dd)
    dd 0x0
    dd 0x0

; Offset Code
; Code Segment Descriptor (8 bytes)
; offset 0x8
gdt_code:     ; CS should point to this
    dw 0xffff ; Segment size (lower 16 bits)
    dw 0x0000   ; Base (Lower 16 bits)
    db 0x00     ; Base (Next 8 bits)
    db 0x9A     ; Access Byte => Defines the type of segment (code/data), privilege level, and access permissions.
    db 0xCF     ; Granularity, 32-bit : Controls granularity, size (16/32-bit), and upper limit bits
    db 0x00     ; Base (Upper 8 bits)   

; offset 0x10
gdt_data:          ; Data Segment Descriptor (8 bytes)
    dw 0xFFFF     ; Limit (low 16 bits)
    dw 0x0000     ; Base (low 16 bits)
    db 0x00       ; Base (next 8 bits)
    db 0x92       ; Access Byte (Data Segment, Read-Write)
    db 0xCF       ; Granularity & Limit (high 4 bits)
    db 0x00       ; Base (high 8 bits)

gdt_end:

gdt_descriptor:  ; we will give that data to lgdt 
    dw gdt_end - gdt_start - 1  ; size
    dd gdt_start  ; Offset

[BITS 32]
load32:
    mov eax, 1  ; Starting Sector no. (0 is for Bootloader)
    mov ecx, 100 ; Total no. of sector 
    mov edi, 0x0100000 ; The address where the sector will be loaded
    call ata_lba_read
    jmp CODE_SEG:0x0100000

; Driver to load kernel
ata_lba_read:
    mov ebx, eax, ; Backup the LBA
    ; Send the highest 8 bits of the lba to hard disk controller
    shr eax, 24
    or eax, 0xE0 ; Select the master drive
    mov dx, 0x1F6
    out dx, al
    ; Finished sending the highest 8 bits of the lba 

    ; Send the total sectors to read
    mov eax, ecx
    mov dx, 0x1F2
    out dx, al
    ; Finished sending the total sectors to read

    mov eax, ebx ; Restore the backup LBA
    mov dx, 0x1F3
    out dx, al
    ; Finished sending more bits of the LBA

    ; Send more bits of the LBA
    mov dx, 0x1F4
    mov eax, ebx ; Restore the backup LBA
    shr eax, 8
    out dx, al
    ; Finished sending more bits of the LBA

    ; Send upper 16 bits of the LBA
    mov dx, 0x1F5
    mov eax, ebx ; Restore the backup LBA
    shr eax, 16
    out dx, al
    ; Finished sending upper 16 bits of the LBA

    mov dx, 0x1f7
    mov al, 0x20
    out dx, al

.next_sector:
    push ecx

; Checking if we need to read
.try_again:
    mov dx, 0x1f7
    in al, dx
    test al, 8
    jz .try_again
 

; We need to read 256 words at a time
    mov ecx, 256
    mov dx, 0x1F0
    rep insw
    pop ecx
    loop .next_sector
    ; End of reading sectors into memory
    ret    

times 510- ($ - $$) db 0
dw 0xAA55 ; Boot sector magic number. This will be 55AA due to little Endian
