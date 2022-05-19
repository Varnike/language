#include <stdio.h>
#include <stdlib.h>
#include "elfhdr.h"

/*
typedef struct elf64_hdr {
  unsigned char	e_ident[EI_NIDENT];
  Elf64_Half e_type;
  Elf64_Half e_machine;
  Elf64_Word e_version;
  Elf64_Addr e_entry;
  Elf64_Off e_phoff;
  Elf64_Off e_shoff;
  Elf64_Word e_flags;
  Elf64_Half e_ehsize;
  Elf64_Half e_phentsize;
  Elf64_Half e_phnum;
  Elf64_Half e_shentsize;
  Elf64_Half e_shnum;
  Elf64_Half e_shstrndx;
} Elf64_Ehdr; 
 */

int setup_elfhdr(Elf64_Ehdr *ehdr)
{
	if (!ehdr)
		return -1;

	*ehdr = Elf64_Ehdr {
		.e_type	     = ET_DYN,
		.e_machine   = EM_X86_64,
		.e_version   = EV_CURRENT,
		.e_entry     = 0x8048080,//EDIT
		.e_phoff     = 64,
		.e_shoff     = 0,
		.e_flags     = 0x0,
		.e_ehsize    = sizeof(Elf64_Ehdr),
		.e_phentsize = sizeof(Elf64_Phdr),
		.e_phnum     = 0x1,
		.e_shentsize = 0x0,
		.e_shnum     = 0x0,
		.e_shstrndx  = 0x0, 
	};

/* 0 */	ehdr->e_ident[EI_MAG0]       = ELFMAG0;
/* 1 */	ehdr->e_ident[EI_MAG1]       = ELFMAG1;
/* 2 */	ehdr->e_ident[EI_MAG2]       = ELFMAG2;
/* 3 */	ehdr->e_ident[EI_MAG3]       = ELFMAG3;
/* 4 */	ehdr->e_ident[EI_CLASS]      = ELFCLASS64;
/* 5 */	ehdr->e_ident[EI_DATA]       = ELFDATA2LSB;
/* 6 */	ehdr->e_ident[EI_VERSION]    = EV_CURRENT;
/* 7 */ ehdr->e_ident[EI_OSABI]      = ELFOSABI_NONE;
/* 8 */ ehdr->e_ident[EI_ABIVERSION] = 0;
/* 9 */ ehdr->e_ident[EI_PAD]        = 0;

	return 0;
}

int setup_phdr(Elf64_Phdr *phdr)
{
	if (!phdr)
		return -1;

	phdr[0] = Elf64_Phdr {
		.p_type   = PT_LOAD,	/* Segment type */
		.p_flags  = PF_X + PF_R,/* Segment flags */
/*E*/		.p_offset = 0x80,	/* Segment file offset */
/*E*/		.p_vaddr  = 0x8048080,	/* Segment virtual address */
		.p_paddr  = 0x0,	/* Segment physical address */
/*E*/		.p_filesz = 0x7,	/* Segment size in file */
/*E*/		.p_memsz  = 0x7,	/* Segment size in memory */
		.p_align  = 0x1000	/* Segment alignment */
	};

#ifdef PHDR_DATA_SEG
	/* UNUSED so far */
	/* if data segment needed */
	phdr[1] = Elf64_Phdr {
		p_type   = PT_LOAD,	/* Segment type */
		p_flags  = PF_W + PF_R,	/* Segment flags */
		p_offset = 0x0,		/* Segment file offset */
		p_vaddr  = 0x0,		/* Segment virtual address */
		p_paddr  = 0x0,		/* Segment physical address */
		p_filesz = 0x0,		/* Segment size in file */
		p_memsz  = 0x0,		/* Segment size in memory */
		p_align  = 0x1000	/* Segment alignment */
	};
#endif
	return 0;
}

