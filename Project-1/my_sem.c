#include "my_sem.h"
#include "my_lock.h"

void my_sem_init(my_sem_t *sem, int init_value){
    sem->value = init_value;
    my_lock_init(&sem->lock_sem);
}

void my_sem_wait(my_sem_t *sem){

    tas_lock(&sem->lock_sem);
    while (sem->value <= 0){
        unlock(&(sem->lock_sem));
        tas_lock(&(sem->lock_sem)); // Re-verrouiller avant de vérifier à nouveau
    }
    sem->value--;
    unlock(&(sem->lock_sem));
}

void my_sem_post(my_sem_t *sem){
    tas_lock(&(sem->lock_sem));
    sem->value++;
    unlock(&(sem->lock_sem));
}