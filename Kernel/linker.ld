ENTRY(start)

KERNEL_VIRTUAL_BASE = 0xc0000000;

PHDRS
{
  boot_text PT_LOAD ;
  boot_bss PT_LOAD ;
  text PT_LOAD ;
  data PT_LOAD ;
  bss PT_LOAD ;
  ksyms PT_LOAD ;
}

SECTIONS
{
    . = KERNEL_VIRTUAL_BASE + 0x00100000;

    start_of_kernel_image = .;

    .boot_text ALIGN(4K) : AT (ADDR(.boot_text) - KERNEL_VIRTUAL_BASE)
    {
        KEEP(*(.boot_text))
        KEEP(*(.multiboot))
    } :boot_text

    .boot_bss ALIGN(4K) (NOLOAD) : AT (ADDR(.boot_bss) - KERNEL_VIRTUAL_BASE)
    {
        KEEP(*(.page_tables))
        KEEP(*(.stack))
        *(.super_pages)
    } :boot_bss

    .text ALIGN(4K) : AT (ADDR(.text) - KERNEL_VIRTUAL_BASE)
    {
        start_of_kernel_text = .;

        start_of_safemem_text = .;
        KEEP(*(.text.safemem))
        end_of_safemem_text = .;
        start_of_safemem_atomic_text = .;
        KEEP(*(.text.safemem.atomic))
        end_of_safemem_atomic_text = .;

        *(.text*)
    } :text

    .unmap_after_init ALIGN(4K) : AT (ADDR(.unmap_after_init) - KERNEL_VIRTUAL_BASE)
    {
        start_of_unmap_after_init = .;
        *(.unmap_after_init*);
        end_of_unmap_after_init = .;

        end_of_kernel_text = .;
    } :text

    .rodata ALIGN(4K) : AT (ADDR(.rodata) - KERNEL_VIRTUAL_BASE)
    {
        start_heap_ctors = .;
        *libkernel_heap.a:*(.ctors)
        end_heap_ctors = .;

        start_ctors = .;
        *(.ctors)
        end_ctors = .;

        *(.rodata*)
    } :data

    .data ALIGN(4K) : AT (ADDR(.data) - KERNEL_VIRTUAL_BASE)
    {
        start_of_kernel_data = .;
        *(.data*)
        end_of_kernel_data = .;
    } :data

    .ro_after_init ALIGN(4K) (NOLOAD) : AT(ADDR(.ro_after_init) - KERNEL_VIRTUAL_BASE)
    {
        start_of_ro_after_init = .;
        *(.ro_after_init);
        end_of_ro_after_init = .;
    } :data

    .bss ALIGN(4K) (NOLOAD) : AT (ADDR(.bss) - KERNEL_VIRTUAL_BASE)
    {
        start_of_kernel_bss = .;
        *(page_tables)
        *(COMMON)
        *(.bss)
        end_of_kernel_bss = .;

        . = ALIGN(4K);
        *(.heap)
    } :bss

    end_of_kernel_image = .;

    .ksyms ALIGN(4K) : AT (ADDR(.ksyms) - KERNEL_VIRTUAL_BASE)
    {
        *(.kernel_symbols)
    } :ksyms
}
