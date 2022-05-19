#ifndef ELFHDR_H
#define ELFHDR_H

#include <elf.h>
/**
 * Setup ELF header of program
 */
int setup_elfhdr(Elf64_Ehdr *ehdr);

/**
 * Setup programm header
 */
int setup_phdr(Elf64_Phdr *phdr);

#endif // ELFHDR_H
