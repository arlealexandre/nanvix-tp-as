#include <nanvix/const.h>
#include <nanvix/sem.h>

#define GETVAL 0
#define SETVAL 1
#define IPC_RMID 2

/**
 * Permet à une série d'opérations de contrôle sur le sémaphore identifié par semid.
 * Ces opérations sont spécifiées par cmd et peuvent être :
 *  - GETVAL, renvoie la valeur actuelle du sémaphore
 *  - SETVAL, définit la valeur du sémaphore sur val
 *  - IPC_RMID, supprime le sémaphore et le détruit s'il n'est plus utilisé
 * 
 * Valeur de retour :
 *  - GETVAL, valeur actuelle du sémaphore
 *  - AUTRES CAS, -1 en cas d'erreur, 0 sinon
*/
PUBLIC int semctl(int semid, int cmd, int val)
{
    switch (cmd)
    {
    case GETVAL:
        return semtab[semid].sem->counter;
        break;
    case SETVAL:
        if (val < -WAIT_MAX || val > WAIT_MAX)
        {
            return -1;
        }
        semtab[semid].sem->counter = val;
        return 0;
        break;
    case IPC_RMID:
        if (semtab[semid].state == FREE)
        {
            return -1;
        }
        semtab[semid].state = FREE;
        return 0;
        break;
    default:
        return -1;
        break;
    }
}