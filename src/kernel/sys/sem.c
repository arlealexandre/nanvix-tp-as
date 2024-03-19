#include <nanvix/const.h>
#include <nanvix/sem.h>

PUBLIC struct sem_state semtab[SEM_MAX];

PUBLIC void sem_init(void) {

    for (int i=0; i<SEM_MAX; i++) {
        semtab[i].state = FREE;
    }
}