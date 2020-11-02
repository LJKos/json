#ifndef PTRCK_H
#define PTRCK_H

#define BLOCK_SIZE 1024

typedef struct ptrck {
    void **alloc_ptr;
    int alloc_ptr_c;
    int alloc_ptr_s;
} Ptrck;

Ptrck *ptrck_init();
void ptrck_del(Ptrck *ptrck);

int ptrck_add(Ptrck *ptrck, void *ptr);
int ptrck_clear(Ptrck *ptrck);

#endif
