#include <nanvix/const.h>
#include <nanvix/sem.h>
#include <nanvix/pm.h>


PUBLIC int semop(int semid, int op) {

    if (op<=0) { // downw, acquire
        semtab[semid].sem->counter+=op;
        if (semtab[semid].sem->counter<0) {
            sleep(&(semtab[semid].sem->waiting_queue), semtab[semid].sem->counter);
        }
    } else { // up, release
        int current_counter = semtab[semid].sem->counter;
        semtab[semid].sem->counter+=op;
        if (current_counter<0 && semtab[semid].sem->counter>0) {
            wakeup(&(semtab[semid].sem->waiting_queue));
        }
    }
}