#include <nanvix/const.h>
#include <nanvix/sem.h>
#include <nanvix/pm.h>

/**
 * Permet d'effectuer des opérations atomiques incrémentant ou décrémentant la variable associée au sémaphore
 * identifié par semid.
 * 
 * Si op<=0 => down() (acquire)
 * si op>0  => up() (release)
 * 
 * Valeur de retour :
 *  - 0 en cas de réussite
 *  - -1 en cas d'erreur
*/
PUBLIC int semop(int semid, int op) {

    if (semtab[semid].state==FREE) {
        return -1;
    }

    if (op<=0) { // down, acquire
        semtab[semid].sem->counter--;
        if (semtab[semid].sem->counter<0) {
            sleep((semtab[semid].sem->waiting_queue), semtab[semid].sem->counter);
        }
    } else { // up, release-
        if (semtab[semid].sem->counter++ == 0) {
            wakeup((semtab[semid].sem->waiting_queue));
        }
    }

    return 0;
}