// BlacklightEVO kernel/gdt.h -- x86 GDT/TSS/paging
// Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

// For some reason all the paging gunk got shoved in here too.

#define GDT_ENTRIES 128

#if (GDT_ENTRIES % 2 != 0) || (GDT_ENTRIES < 6)
	#error "GDT_ENTRIES is invalid!"
#endif

#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))

typedef struct {
	unsigned short limit_0_15;
	unsigned short base_0_15;
	unsigned char base_16_23;
	unsigned char access;
	unsigned char flags;
	unsigned char base_24_31;
} __attribute__((packed)) gdt_entry;

typedef struct {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed)) gdtr_entry;

extern gdt_entry gdt[GDT_ENTRIES];
extern gdtr_entry gdtr;

// Unmarked entries are unused.
typedef struct {
	unsigned int backlink;	// Backlink to the previous TSS in a hardware context switching linked list.
	unsigned int esp0;		// Kernel mode ESP
	unsigned int ss0;		// Kernel mode SS
	unsigned int esp1;
	unsigned int ss1;
	unsigned int esp2;
	unsigned int ss2;
	unsigned int cr3;
	unsigned int eip;
	unsigned int eflags;
	unsigned int eax;
	unsigned int ecx;
	unsigned int edx;
	unsigned int ebx;
	unsigned int esp;
	unsigned int ebp;
	unsigned int esi;
	unsigned int edi;
	unsigned int es;		// Kernel mode ES
	unsigned int cs;		// Kernel mode CS
	unsigned int ss;		// Kernel mode SS, again
	unsigned int ds;		// Kernel mode DS
	unsigned int fs;		// Kernel mode FS
	unsigned int gs;		// Kernel mode GS
	unsigned int ldt;
	unsigned short reserved1;
	unsigned short iomap_base;
} __attribute__((packed)) tss_entry;

extern bool gdt_used[GDT_ENTRIES / 2];
extern unsigned short gdt_kernel_cs;
extern unsigned short gdt_user_cs;

void gdt_reload_tr(void);
extern void gdt_reload(void);	// from kernel/entry.asm
void gdt_initialize(void);
void gdt_add_selector(int offset, unsigned int base, unsigned int limit, unsigned char access, unsigned char flags);
unsigned short gdt_add_task(unsigned int base, unsigned int limit, bool kernel_mode);

// TSS CRUD

void* tss_get_esp0(void);
void tss_set_esp0(void* new_esp0);

// PAGING CRUD
typedef struct {
	unsigned int present:1;
	unsigned int rw:1;
	unsigned int user:1;
	unsigned int accessed:1;
	unsigned int dirty:1;
	unsigned int unused:7;
	unsigned int frame:20;
} __attribute__((packed)) page_entry;

typedef struct {
	page_entry pages[1024];
} page_table;

typedef struct {
	unsigned int phys_tables[1024];
	page_table* tables[1024];
	unsigned int phys_addr;
} page_directory;

extern page_table paging_kernel_tables[1024];

extern page_directory* paging_kernel_directory;
extern page_directory* paging_current_directory;

extern unsigned int* paging_frames;
extern unsigned int paging_nframes;

#define PAGING_ENTRY_PTR(x) (void*)((unsigned int)x & 0xFFFFF000)

void paging_make_dentry(unsigned int* dentry, bool user);
void paging_set_directory(page_directory* directory);
