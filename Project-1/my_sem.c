#include "my_sem.h"
#include "my_lock.h"

/*
 * Initialise la valeur du sémaphore à init_value et initialise le verrou associé.
 */
void my_sem_init(my_sem_t *sem, int init_value){
    sem->value = init_value;
    my_lock_init(&sem->lock_sem);
}

/*
 * Attends que la valeur du sémaphore soit nulle ou positive
 * et la décrémente de 1.
 */
void my_sem_wait(my_sem_t *sem){

    tatas_lock(&sem->lock_sem);
    while (sem->value <= 0){
        unlock(&(sem->lock_sem));
        tatas_lock(&(sem->lock_sem)); // Re-verrouiller avant de vérifier à nouveau.
    }
    sem->value--;
    unlock(&(sem->lock_sem));
}

/*
 * Incrémente la valeur du sémaphore de 1.
 */
void my_sem_post(my_sem_t *sem){
    tatas_lock(&(sem->lock_sem));
    sem->value++;
    unlock(&(sem->lock_sem));
}