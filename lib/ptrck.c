
#include "ptrck.h"

#include <stdio.h>
#include <stdlib.h>

Ptrck *ptrck_init() {
    Ptrck *ptrck = malloc(sizeof(Ptrck));
    if (!ptrck)
        return NULL;
    
    ptrck->alloc_ptr_c = 0;
    ptrck->alloc_ptr_s = BLOCK_SIZE;
    
    ptrck->alloc_ptr = malloc(sizeof(void *) * ptrck->alloc_ptr_s);
    if (!ptrck->alloc_ptr) {
        free(ptrck);
        return NULL;
    }
    
    return ptrck;
}



void ptrck_del(Ptrck *ptrck) {
    free(ptrck->alloc_ptr);
    free(ptrck);
}



int ptrck_add(Ptrck *ptrck, void *ptr) {
    if (ptrck->alloc_ptr_c >= ptrck->alloc_ptr_s) {
        ptrck->alloc_ptr_s += BLOCK_SIZE;
        void **alloc_ptr_tmp = realloc(ptrck->alloc_ptr, sizeof(void *) * ptrck->alloc_ptr_s);
        if (alloc_ptr_tmp == NULL) {
            return -1;
        }
        ptrck->alloc_ptr = alloc_ptr_tmp;
    }
    
    ptrck->alloc_ptr[ptrck->alloc_ptr_c] = ptr;
    ptrck->alloc_ptr_c++;
    return 0;
}



int ptrck_clear(Ptrck *ptrck) {
    for (int i = 0; i < ptrck->alloc_ptr_c; i++) {
        free(ptrck->alloc_ptr[i]);
    }
    
    ptrck->alloc_ptr_s = BLOCK_SIZE;
    void **alloc_ptr_tmp = realloc(ptrck->alloc_ptr, sizeof(void *) * ptrck->alloc_ptr_s);
    if (alloc_ptr_tmp == NULL) {
        return -1;
    }
    ptrck->alloc_ptr = alloc_ptr_tmp;
    
    return 0;
}
