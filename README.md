# nanvix-tp-as

##### Membres du groupe
- Alexandre Arle (arlea)
- Rémi Del Medico (delmedir)
- Emin Gundogan (gundogae)

## Process Scheduling
Nous avons principalement modifié les fichiers [sched.c](https://github.com/arlealexandre/nanvix-tp-as/blob/p1_process_scheduling/src/kernel/pm/sched.c), [fork.c](https://github.com/arlealexandre/nanvix-tp-as/blob/p1_process_scheduling/src/kernel/sys/fork.c), [pm.h](https://github.com/arlealexandre/nanvix-tp-as/blob/p1_process_scheduling/include/nanvix/pm.h) sur la branche p1_process_scheduling. 

Nous avons implémenté les politiques d'ordonnancement suivantes :
- Loterie
- Multiple Queue
- Priorité
- Shortest Job First (SJF)