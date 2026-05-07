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

int test_getwmap1();
int test_getpgdirinfo1();

void readwmapinf(struct wmapinfo *wp, struct proc *p){

}

// print all details of pgdirinfo
void readpgdirinf(struct pgdirinfo *pd, int totalmaps){
    for(int i = 0; i <totalmaps;i++){
        printf("Pages Allocated: %d\n",pd->n_upages);
        printf("Virtual Address: %d\n",pd->va[i]);
        printf("Physical Address: %d\n",pd->pa[i]); 
    }
}

int main(){
    test_getwmap1();
    test_getpgdirinfo1();

    // takes care of segmentation fault
    exit();
    return 0;
}

// successful mapping
int test_pgd1(){
    return 0;
}

// test for getwmapinfo
int test_getwmap1(){
    // how to test for more or less ptes?
    struct wmapinfo *wm;
    getwmapinfo(wm);
    return 0;

}

// test for getpgdirinfo
int test_getpgdirinfo1(){
    struct pgdirinfo *pd;
    getpgdirinfo(pd);
    return 0;
}