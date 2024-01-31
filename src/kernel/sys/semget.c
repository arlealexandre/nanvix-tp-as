#include <nanvix/const.h>
#include <nanvix/sem.h>

PUBLIC int semget(int key) {

    int empty_index = -1;

    for (int i=0; i<SEM_MAX; i++) {

        if (semtab[i].state == FULL) {
            if (semtab[i].sem->key == key) {
                return i;
            }
        } else if (empty_index==-1){
            empty_index = i;
        }
    }

    if (empty_index!=-1) {

        struct sem new_sem;

        new_sem.counter=0;
        new_sem.key=key;
        new_sem.next_index_waiting=0;

        semtab[empty_index].sem = &new_sem;

    }

    return empty_index;

}