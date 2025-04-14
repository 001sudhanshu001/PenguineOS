[BITS 32]

section .asm

global paging_load_directory
global enable_paging


paging_load_directory:
    push ebp
    mov ebp, esp
    mov eax, [ebp+8] ; get first argument (pointer to directory)
    mov cr3, eax       ; load it into CR3 register (which tells CPU where page directory is)
    pop ebp
    ret

    
enable_paging:
    push ebp
    mov ebp, esp
    mov eax, cr0
    or eax, 0x80000000 ; set the PG (Paging Enable) bit (bit 31)
    mov cr0, eax
    pop ebp
    ret

