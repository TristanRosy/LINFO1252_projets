#ifndef TEST_AND_SET_H
#define TEST_AND_SET_H

#include <stdbool.h>

typedef struct {
    int lock; // Variable pour le verrou par attente active
} my_lock_t;

bool xchg(int *addr, int value);

void my_lock_init(my_lock_t *lock_var);

void tas_lock(my_lock_t *lock_var);

void tatas_lock(my_lock_t *lock_var);

void bo_tatas_lock(my_lock_t *lock_var);

void lock_release(my_lock_t *lock_var);

void lock_manager_acquire(my_lock_t *lock, int num_sections);

void unlock(my_lock_t *lock_var);

#endif /* TEST_AND_SET_H */
