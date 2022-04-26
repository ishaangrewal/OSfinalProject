#include "elf.h"
#include "debug.h"

uint32_t ELF::load(Shared<Node> file) {
    char* buffer = new char[sizeof(ElfHeader)];
    file->read_all(0, sizeof(ElfHeader), buffer);
    ElfHeader* elf = (ElfHeader*)buffer;
    if (elf->maigc0 != 0x7F || elf->magic1 != 'E' || elf->magic2 != 'L' || elf->magic3 != 'F') {
        Debug::panic("error");
    }
    char* second_buffer = new char[elf->phnum * elf->phentsize];
    file->read_all(elf->phoff, elf->phnum * elf->phentsize, second_buffer);
    ProgramHeader* progh = (ProgramHeader*)second_buffer;
    ProgramHeader cur;
    for (int i = 0 ; i < elf->phnum; i++) {
        cur = progh[i];
        if (cur.type == 1) {
            file->read_all(cur.offset, cur.filesz, (char*)cur.vaddr);
        }
    }
    return elf->entry;
}


















































#if 0
    ElfHeader hdr;

    file->read(0,hdr);

    uint32_t hoff = hdr.phoff;

    for (uint32_t i=0; i<hdr.phnum; i++) {
        ProgramHeader phdr;
        file->read(hoff,phdr);
        hoff += hdr.phentsize;

        if (phdr.type == 1) {
            char *p = (char*) phdr.vaddr;
            uint32_t memsz = phdr.memsz;
            uint32_t filesz = phdr.filesz;

            Debug::printf("vaddr:%x memsz:0x%x filesz:0x%x fileoff:%x\n",
                p,memsz,filesz,phdr.offset);
            file->read_all(phdr.offset,filesz,p);
            bzero(p + filesz, memsz - filesz);
        }
    }

    return hdr.entry;
#endif
