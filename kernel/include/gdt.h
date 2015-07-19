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

extern bool gdt_used[GDT_ENTRIES / 2];
extern unsigned short gdt_kernel_cs;

extern void gdt_reload(void);	// from kernel/entry.asm
void gdt_initialize(void);
void gdt_add_selector(int offset, unsigned int base, unsigned int limit, unsigned char access, unsigned char flags);
unsigned short gdt_add_task(unsigned int base, unsigned int limit, bool kernel_mode);

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
