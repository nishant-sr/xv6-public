#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "wmap.h"
#define PAGE_INCREMENT 0x1000
#define PAGE_SIZE 4096

// uint, int, int, int => uint
// virtual address for mapping, length of mapping in bytes
// length > 0 , flags can be ORed together
uint wmap(uint addr, int length, int flags, int fd){

    if (length < 0){
        exit();
    }

    //if we have 1 byte, how many pages is that?
    int pages = length / PGSIZE;
    if ((length % PGSIZE) > 0){
        pages += 1;
    }

    struct proc *p = myproc();

    // insufficient memory
    if (p->total_mmaps == MAX_WMMAP_INFO){
        cprintf("full of memory for this process");
        exit();
    }

    // address validity
    if (addr > 0x80000000 || addr < 0x60000000){
        cprintf("invalid addr, not in range");
        exit();
    }

    uint newend = addr + length;

    // check if there is nothing in the specified range of [addr,add+length]
    for(int i = 0; i < p->total_mmaps;i++){
        uint start = p->addr[i];
        uint end = start + p->length[i];
        
        // requested slot and size is not fully available
        if (!(end <= addr) || !(start >= newend)){
            exit();
        }
    }

    // slot fully open
    int newmap = p->total_mmaps;
    p->addr[newmap] = addr;
    p->length[newmap] = length;
    p->n_loaded_pages[newmap] = pages;
    p->total_mmaps++;

    return addr;
}

int wunmap(uint addr){
    struct proc *p = myproc();

    for(int i = 0; i < p->total_mmaps;i++){
        uint start = p->addr[i];
        uint end = start + p->length[i];
        
        // requested slot and size is not fully available
        if (addr == start){
            
            for(int j = 0; j < p->n_loaded_pages[i];j++){
                uint newaddr = addr + PAGE_INCREMENT * j;
                pte_t *pte = walkpgdir(p->pgdir, (void*)newaddr, 0);
                uint physical_address = PTE_ADDR(*pte);
                kfree(P2V(physical_address));
                *pte = 0;
            }
            
            p->addr[i] = 0;
            p->length[i] = 0;
            p->total_mmaps--;
            p->n_loaded_pages[i] = 0;

            return 0;
        }
    }

    return -1;
}

uint registeredwmap(uint address){
    struct proc *p = myproc();

    for(int i = 0; i < p->total_mmaps; i++){
        if(address >= p->addr[i] && address < p->addr[i] + p->length[i]){
            return 0;
        }
    }

    return -1;

}

int updatepagetable(uint address){
    struct proc *p = myproc();

    char *mem = kalloc();
    mappages(p->pgdir, address, PAGE_SIZE, V2P(mem), PTE_W | PTE_U);

    return 0;
}

// iterate page directory and record pa and va of all memory maps
int getwmapinfo(struct wmapinfo *wminfo){
    struct proc *p = myproc();
    pde_t *pde = p->pgdir;
    pte_t *pgtab;
    for(int i = 0; i<MAX_UPAGE_INFO;i++){
        cprintf("%d",&pde[i]);
        pde++;
    }
    return 0;
}

int getpgdirinfo(struct pgdirinfo *pdinfo){
    struct proc *p = myproc();
    for(int i = 0; i <p->total_mmaps;i++){
        pdinfo->n_upages += p->n_loaded_pages[i];
        pdinfo->va[i] = p->addr[i];
        pdinfo->pa[i] = (uint) walkpgdir(p->pgdir, (void*)p->addr[i],PAGE_SIZE);
    }
    return 0;
}