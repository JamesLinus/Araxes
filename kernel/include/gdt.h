// BlacklightEVO kernel/gdt.h -- x86 GDT/TSS/paging
// Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
// The Blacklight project is under the terms of the ISC license. See license.md for details.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

// For some reason all the paging gunk got shoved in here too.

#define GDT_ENTRIES 128

#if (GDT_ENTRIES % 2 != 0) || (GDT_ENTRIES < 6)
	#error "GDT_ENTRIES is invalid!"
#endif

#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))

typedef struct {
	uint16_t limit_0_15;
	uint16_t base_0_15;
	uint8_t base_16_23;
	uint8_t access;
	uint8_t flags;
	uint8_t base_24_31;
} __attribute__((packed)) gdt_entry;

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) gdtr_entry;

extern gdt_entry gdt[GDT_ENTRIES];
extern gdtr_entry gdtr;

// Unmarked entries are unused.
typedef struct {
	uint32_t backlink;				// Backlink to the previous TSS in a hardware context switching linked list.
	uint32_t esp0;				// Kernel mode ESP
	uint32_t ss0;				// Kernel mode SS
	uint32_t esp1;
	uint32_t ss1;
	uint32_t esp2;
	uint32_t ss2;
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint32_t es;				// Kernel mode ES
	uint32_t cs;				// Kernel mode CS
	uint32_t ss;				// Kernel mode SS, again
	uint32_t ds;				// Kernel mode DS
	uint32_t fs;				// Kernel mode FS
	uint32_t gs;				// Kernel mode GS
	uint32_t ldt;
	uint16_t reserved1;
	uint16_t iomap_base;
} __attribute__((packed)) tss_entry;

extern bool gdt_used[GDT_ENTRIES / 2];
extern uint16_t gdt_kernel_cs;
extern uint16_t gdt_user_cs;

void gdt_reload_tr(void);
extern void gdt_reload(void);				// from kernel/entry.asm
void gdt_initialize(void);
void gdt_add_selector(int offset, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags);
uint16_t gdt_add_task(uint32_t base, uint32_t limit, bool kernel_mode);

// TSS CRUD

void* tss_get_esp0(void);
void tss_set_esp0(void* new_esp0);

// PAGING CRUD
typedef struct {
	uint32_t present:1;
	uint32_t rw:1;
	uint32_t user:1;
	uint32_t accessed:1;
	uint32_t dirty:1;
	uint32_t unused:7;
	uint32_t frame:20;
} __attribute__((packed)) page_entry;

typedef struct {
	page_entry pages[1024];
} page_table;

typedef struct {
	uint32_t phys_tables[1024];
	page_table* tables[1024];
	uint32_t phys_addr;
} page_directory;

extern page_table paging_kernel_tables[1024];

extern page_directory* paging_kernel_directory;
extern page_directory* paging_current_directory;

extern uint32_t* paging_frames;
extern uint32_t paging_nframes;

#define PAGING_ENTRY_PTR(x) (void*)((uint32_t)x & 0xFFFFF000)

void paging_make_dentry(uint32_t* dentry, bool user);
void paging_set_directory(page_directory* directory);
