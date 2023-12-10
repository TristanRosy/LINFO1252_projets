#ifndef LINFO1252_PROJETS_MY_SEM_H
#define LINFO1252_PROJETS_MY_SEM_H

#include "my_lock.h"

typedef struct {
    int value; // Valeur du sémaphore.
    my_lock_t lock_sem; // Verrou associé au sémaphore.
} my_sem_t;

void my_sem_init(my_sem_t *sem, int init_value);

void my_sem_wait(my_sem_t *sem);

void my_sem_post(my_sem_t *sem);

#endif //LINFO1252_PROJETS_MY_SEM_H
