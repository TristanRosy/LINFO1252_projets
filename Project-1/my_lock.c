#include "my_lock.h"

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

void my_lock_init(my_lock_t *lock_var){
    lock_var->lock = 0;
}

// test and set lock
void tas_lock(my_lock_t *lock_var) {
    while (xchg(&(lock_var->lock), 1)) {
        // Attente active
    }
}

// test and test and set lock
void tatas_lock(my_lock_t *lock_var) {
    while (xchg(&(lock_var->lock), 1)) {
        // Attente active
        while(lock_var->lock){}
    }
}

// backoff test and test and set lock
void bo_tatas_lock(my_lock_t *lock_var) {
    int delay = 10;
    while (xchg(&lock_var->lock, 1)) {
        // Attente active

        while(lock_var->lock){
            for (int i = 0; i < delay; ++i);
            if (delay < 1000000000000) delay *= 10;
        }
    }
}

void unlock(my_lock_t *lock_var) {
    __asm__ volatile(
            "lock xchg %1, %0 ;"
            : "+m" (*(&lock_var->lock))
            : "r" (0)
            : "memory"
            );
}