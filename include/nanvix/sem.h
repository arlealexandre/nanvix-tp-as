#ifndef NANVIX_SEM_H_
#define NANVIX_SEM_H_

#include <nanvix/pm.h>
#include <nanvix/const.h>

#define SEM_MAX 64
#define WAIT_MAX 64

#define FREE 0
#define FULL 1

struct sem
{
    unsigned key; // utiliser pour partager les processus
    int counter; // compteur du sémaphore
    struct process * waiting_queue[WAIT_MAX]; // liste d'attente des processus
    int next_index_waiting;
};

struct sem_state {
    unsigned int state;
    struct sem * sem;
};

/* Tableau de sémaphores */
EXTERN struct sem_state semtab[SEM_MAX];

/* Fonction d'initialisation */
EXTERN void sem_init(void);

#endif /* NANVIX_SEM_H_ */
