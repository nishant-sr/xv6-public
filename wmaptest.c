#include "wmap.h"

int test_getwmap1();
int test_getpgdirinfo1();

int main(){
    
    test_getwmap1();
    test_getpgdirinfo1();

}

// test for getwmapinfo
int test_getwmap1(){

    struct wmapinfo *wm;
    return getwmapinfo(wm);

}

// test for getpgdirinfo
int test_getpgdirinfo1(){

    struct pgdirinfo *pd;
    return getpgdirinfo(pd);

}