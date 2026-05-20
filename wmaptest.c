#include "types.h"
#include "user.h"
#include "fcntl.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "wmap.h"
#define PAGE_INCREMENT 0x1000
#define PAGE_SIZE 4096

int test_getwmap1();
int test_getpgdirinfo1();

void readwmapinf(struct wmapinfo *wp, struct proc *p){

}

// print all details of pgdirinfo
void readpgdirinf(struct pgdirinfo *pd, int totalmaps){
    for(int i = 0; i <totalmaps;i++){
        printf(1,"Pages Allocated: %d\n",pd->n_upages);
        printf(1,"Virtual Address: %d\n",pd->va[i]);
        printf(1,"Physical Address: %d\n",pd->pa[i]); 
    }
}

int main(){
    printf(1,"TEST 1 A\n\n");
    test_getwmap1();
    printf(1,"\n");

    printf(1,"TEST 1 B\n\n");
    test_getwmap2();
    printf(1,"\n");

    printf(1,"TEST 2 A\n\n");
    test_getpgdirinfo1();
    printf(1,"\n");

    printf(1,"TEST 2 B\n\n");
    test_getpgdirinfo2();
    printf(1,"\n");
    
    printf(1,"TEST 2 C\n\n");
    test_getpgdirinfo3();
    printf(1,"\n");

    // takes care of segmentation fault
    exit();
    return 0;
}

// successful mapping
int test_pgd1(){
    return 0;
}

// addr > 0x80000000 || addr < 0x60000000
// test for getwmapinfo
int test_getwmap1(){
    struct wmapinfo *wm;
    int numpages = 1;
    int extra = 250;

    wmap(0x60011000, PAGE_SIZE * numpages + extra, MAP_ANONYMOUS | MAP_FIXED | MAP_SHARED, -1);
    wmap(0x60012000, 20, MAP_ANONYMOUS | MAP_FIXED | MAP_SHARED, -1);

    getwmapinfo(&wm);

    for(int i = 0; i< wm->total_mmaps; i++){
        printf(1,"Address: %x\t",wm->addr[i]);
        printf(1,"Length: %d\t",wm->length[i]);
        printf(1,"Pages: %d\n",wm->n_loaded_pages[i]);
    }

    return 0;

}

int test_getwmap2(){
    struct wmapinfo *wm;
    int numpages = 2;
    int extra = 250;
    int fd = open("wmapfile.txt", O_RDWR);
    printf(1,"fd: %d\n",fd);
    
    wmap(0x70044000, PAGE_SIZE * numpages + extra, MAP_ANONYMOUS | MAP_FIXED | MAP_SHARED, -1);
    getwmapinfo(&wm);

    for(int i = 0; i< wm->total_mmaps; i++){
        printf(1,"Address: %x\t",wm->addr[i]);
        printf(1,"Length: %d\t",wm->length[i]);
        printf(1,"Pages: %d\n",wm->n_loaded_pages[i]);
    }

    printf(1,"testing testing\n");

    return 0;
}

// pgdirinfo tests
int test_getpgdirinfo1(){
    struct pgdirinfo *pd;
    int numslots = 1;
    getpgdirinfo(pd);
    printf(1,"# of Allocated Pages: %d\n", pd->n_upages);

    return 0;
}

// no malloc

// static memory

// below is for when we do sizeof int
// every 10,000 gives us 10 pages, but not every 1000 gives us 1 page
int test_getpgdirinfo2(){
    struct pgdirinfo *pd;
    int numslots = 40000;
    int *ptr = malloc(numslots * sizeof(int));
    if(ptr == 0){
        return 1;
    }
    for(int i = 0; i<numslots;i++){
        ptr[i] = i * 99;
    }
    getpgdirinfo(pd);
    printf(1,"# of Allocated Pages: %d\n", pd->n_upages);
    return 0;
}

// adding 1 page
int test_getpgdirinfo3(){
    struct pgdirinfo *pd;
    int numpages = 3;
    int numslots = PAGE_SIZE * numpages;
    int *ptr = malloc(numslots * 1);
    if(ptr == 0){
        return 1;
    }
    getpgdirinfo(pd);
    printf(1,"# of Allocated Pages: %d\n", pd->n_upages);
    return 0;
}