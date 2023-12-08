#include "my_lock.h"

/*
 * Echange atomique entre value
 * et la valeur à l'adresse addr et retourne true si l'échange a réussi.
 */
bool xchg(int *addr, int value){
    int result;
    __asm__ volatile(
            "lock xchg %1, %0 ;"
            : "=r" (result), "+m" (*addr)
            : "r" (value)
            : "memory"
            );
    return result != 0;
}

/*
 * Initialisation du verrou à 0 pour indiquer qu'il est libre.
 */
void my_lock_init(my_lock_t *lock_var){
    lock_var->lock = 0;
}

/*
 * Lock du Test-and-Set.
 */
void tas_lock(my_lock_t *lock_var) {
    while (xchg(&(lock_var->lock), 1)) {
        // Attente active
    }
}

/*
 * Lock du Test-and-Test-and-Set.
 */
void tatas_lock(my_lock_t *lock_var) {
    while (xchg(&(lock_var->lock), 1)) {

        while(lock_var->lock){} // Attente active
    }
}

/*
 * Lock du Back Off Test-and-Test-and-Set.
 */
void bo_tatas_lock(my_lock_t *lock_var) {
    int delay = 100;
    while (xchg(&lock_var->lock, 1)) {

        while(lock_var->lock){ // Attente active
            for (int i = 0; i < delay; ++i);
            if (delay < 10000000) delay *= 10; // Le délai est incrémenté jusqu'à un maximum.
        }
    }
}

/*
 * Echange atomique pour remettre le verrou à 0.
 */
void unlock(my_lock_t *lock_var) {
    __asm__ volatile(
            "lock xchg %1, %0 ;"
            : "+m" (*(&lock_var->lock))
            : "r" (0)
            : "memory"
            );
}