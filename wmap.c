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

    if ((length <= 0) || !(flags & MAP_FIXED)){
        return -1;
    }

    if (flags & MAP_ANONYMOUS){
        // ignores FD regardless of MAP PRIVATE or not
        fd = -1;
    }
    // file-backed
    else if (flags & MAP_PRIVATE){
    //if we have 1 byte, how many pages is that?
        cprintf("Private Mapping\n");
    }


    int pages = length / PGSIZE;
    if ((length % PGSIZE) > 0){
        pages += 1;
    }

    struct proc *p = myproc();

    // insufficient memory
    if (p->total_mmaps == MAX_WMMAP_INFO){
        cprintf("full of memory for this process\n");
        return -1;
    }

    // address validity
    if (addr > 0x80000000 || addr < 0x60000000){
        cprintf("invalid addr, not in range\n");
        return -1;
    }

    if((addr % PAGE_SIZE) !=0){
        cprintf("Not page aligned\n");
        return -1;
    }

    uint newend = addr + length;

    // check if there is nothing in the specified range of [addr,add+length]
    for(int i = 0; i < p->total_mmaps;i++){
        uint start = p->addr[i];
        uint end = start + p->length[i];
        
        // requested slot and size is not fully available
        if (((start<=addr)&&(addr<=end)) || ((start<=newend)&&(newend<=end))){
            cprintf("Invalid area, [%x : %x] ;  -- %x : %x\n", start, end, addr, newend);
            return -1;
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
        // uint end = start + p->length[i];
        
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

    // couldn't find the slot
    return -1;
}

uint registeredwmap(uint address){
    struct proc *p = myproc();

    for(int i = 0; i < p->total_mmaps; i++){
        if(address >= p->addr[i] && address < p->addr[i] + p->length[i]){
            return 0;
        }
    }

    cprintf("not registered\n");

    return -1;

}

int updatepagetable(uint address){
    struct proc *p = myproc();

    char *mem = kalloc();
    mappages(p->pgdir, (void*)address, PAGE_SIZE, V2P(mem), PTE_W | PTE_U);

    return 0;
}

// iterate page directory and record pa and va of all memory maps
int getpgdirinfo(struct pgdirinfo *pd){
    struct proc *p = myproc();
    pde_t *pde = p->pgdir;
    pte_t *pte;

    // cprintf("Process:%s\n",p->name);
    // cprintf("PAGE DIRECTORY ADDRESS: %x\n", pde);

    

    int validpages = 0;

    for(int i = 0; i<NPDENTRIES;i++){
        // cprintf("PDE: %x\n", pde[i]);
        if(pde[i] & PTE_P){
            
            // cprintf("PDE Physical Address: %x\n", pde[i]);

            // translation using the pde[i] ?
            // we need PTE to be pointing to the first PTE of the table/pde

            // PDE holds physical addresses, so convert to VA of PTE[0]
            pte = (pte_t*)P2V(PTE_ADDR(pde[i]));

            for(int j = 0; j < NPTENTRIES; j++){
                if((pte[j] & PTE_P) && (pte[j] & PTE_U)){
                    // cprintf("\tPTE Virtual Address: %x\n", pte[j]);
                    // cprintf("\tPTE Physical Address: %x\n",V2P(pte[j]));
                    validpages += 1;
                }
            }

        }

    }

    pd->n_upages = validpages;
    return 0;
}

int getwmapinfo(struct wmapinfo *wminfo){
    struct proc *p = myproc();

    for(int i = 0; i <p->total_mmaps;i++){
        wminfo->addr[i] = p->addr[i];
        wminfo->length[i] = p->length[i];
        wminfo->n_loaded_pages[i] = p->n_loaded_pages[i];
    }
    
    cprintf("inside getwmapinfo\n");
    wminfo->total_mmaps = p->total_mmaps;

    return 0;
}