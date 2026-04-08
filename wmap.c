#include "types.h"
#include "wmap.h"
#include "proc.h"
#include "mmu.h"
#define PAGE_INCREMENT 0x1000

uint wmap(uint addr, int length, int flags, int fd);
int wunmap(uint addr);
uint wremap(uint oldaddr, int oldsize, int newsize, int flags);

// uint, int, int, int => uint
// virtual address for mapping, length of mapping in bytes
// length > 0 , flags can be ORed together
uint wmap(uint addr, int length, int flags, int fd){
    if (length < 0){
        exit(1);
    }

    //if we have 1 byte, how many pages is that?
    int pages = length / PGSIZE;
    if ((length % PGSIZE) > 0){
        pages += 1;
    }

    struct proc *p = myproc();

    for(int i = 0; i <pages; i++){
        char *mem = kalloc();
        uint newaddr = addr + (PAGE_INCREMENT * i);
        mappages(p->pgdir, newaddr, 4096, V2P(mem), PTE_W | PTE_U);

    }

    return 0;
}