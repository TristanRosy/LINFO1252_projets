#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "my_lock.h"

#define CYCLES 1000000 // Nombre de cycles par philosophe.
int PHILOSOPHES; // Nombre de philosophes

my_lock_t *baguette;

/*
 * Fonction de manger du philosophe id
 */
void mange(int id) {
    //printf("Philosophe [%d] mange\n", id);
}

/*
 * Fonction philosophe
 */
void* philosophe(void* arg) {

    int *id = (int *)arg;
    int left = *id;
    int right = (left + 1) % PHILOSOPHES;

    for (int i = 0; i < CYCLES; i++) {
        // Actions de penser sans attente

        // Acquérir les baguettes dans l'ordre des adresses.
        if (left < right) {
            tas_lock(&baguette[left]);
            tas_lock(&baguette[right]);
        } else {
            tas_lock(&baguette[right]);
            tas_lock(&baguette[left]);
        }

        mange(*id);

        // Relâcher les baguettes dans l'ordre inverse des adresses.
        unlock(&baguette[right]);
        unlock(&baguette[left]);
    }

    return NULL;
}

int main(int argc, char *argv[]) {

    // Lecture du premier argument du programme.
    PHILOSOPHES = atoi(argv[1]);;

    pthread_t phil[PHILOSOPHES];

    // Allocation dynamique pour les mutex.
    baguette = (my_lock_t *)malloc(sizeof(my_lock_t) * PHILOSOPHES);
    for (int i = 0; i < PHILOSOPHES; ++i) {
        my_lock_init(&baguette[i]);
    }

    for (int i = 0; i < PHILOSOPHES; ++i) {
        int* id = malloc(sizeof(int)); // Alloue dynamiquement un ID pour chaque philosophe.
        *id = i;
        pthread_create(&phil[i], NULL, philosophe, id);
    }

    for (int i = 0; i < PHILOSOPHES; ++i) {
        pthread_join(phil[i], NULL);
    }

    // Libération des mutex.
    free(baguette);

    return 0;
}
