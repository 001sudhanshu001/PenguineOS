ORG 0x7C00
BITS 16

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

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
; We can no longer access the BIOS, so we have to write our own Disk Driver tor ead from Disk
load32:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov ebp, 0x00200000
    mov esp, ebp
    jmp $


times 510- ($ - $$) db 0
dw 0xAA55 ; Boot sector magic number. This will be 55AA due to little Endian
