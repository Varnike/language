#ifndef ELFHDR_H
#define ELFHDR_H

#include <elf.h>

int setup_elfhdr(Elf64_Ehdr *ehdr);
int setup_phdr(Elf64_Phdr *phdr);

#endif // ELFHDR_H
