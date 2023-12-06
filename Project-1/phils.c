#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

int PHILOSOPHES; // Nombre de philosophes
#define CYCLES 1000000

pthread_mutex_t *baguette;

void mange(int id) {
    // Actions de manger sans attente
    //printf("Philosophe [%d] mange\n", id);
    // Les actions effectuées pour manger
}

void* philosophe(void* arg) {
    int *id = (int *)arg;
    int left = *id;
    int right = (left + 1) % PHILOSOPHES;

    for (int i = 0; i < CYCLES; i++) {
        // Actions de penser sans attente
        // Les actions effectuées pour penser

        // Acquérir les baguettes dans l'ordre des adresses
        if (left < right) {
            pthread_mutex_lock(&baguette[left]);
            pthread_mutex_lock(&baguette[right]);
        } else {
            pthread_mutex_lock(&baguette[right]);
            pthread_mutex_lock(&baguette[left]);
        }

        mange(*id);

        // Relâcher les baguettes dans l'ordre inverse des adresses
        pthread_mutex_unlock(&baguette[right]);
        pthread_mutex_unlock(&baguette[left]);
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Veuillez spécifier le nombre de philosophes.\n");
        return 1;
    }

    int PHILOSOPHERS = atoi(argv[1]);
    PHILOSOPHES = PHILOSOPHERS;
    pthread_t phil[PHILOSOPHERS];

    // Allocation dynamique pour les mutex
    baguette = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t) * PHILOSOPHERS);
    for (int i = 0; i < PHILOSOPHERS; ++i) {
        pthread_mutex_init(&baguette[i], NULL);
    }

    for (int i = 0; i < PHILOSOPHERS; ++i) {
        int* id = malloc(sizeof(int)); // Alloue dynamiquement un ID pour chaque philosophe
        *id = i;
        pthread_create(&phil[i], NULL, philosophe, id);
    }

    for (int i = 0; i < PHILOSOPHERS; ++i) {
        pthread_join(phil[i], NULL);
    }

    // Libération des mutex
    for (int i = 0; i < PHILOSOPHERS; ++i) {
        pthread_mutex_destroy(&baguette[i]);
    }
    free(baguette);

    return 0;
}
