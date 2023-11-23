#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

pthread_t *phil;
pthread_mutex_t *baguette;
int N;

void mange(int id) {
    printf("Philosophe [%d] mange\n", id);
}

void *philosophe(void *arg) {
    int *id = (int *)arg;
    int left = *id;
    int right = (left + 1) % N;
    int first = *id % 2; // Utilisation de modulo pour différencier les philosophes pairs et impairs

    for (int i = 0; i < 10000; ++i) {
        // philosophe pense
        if (first == 0) {
            pthread_mutex_lock(&baguette[left]);
            pthread_mutex_lock(&baguette[right]);
        } else {
            pthread_mutex_lock(&baguette[right]);
            pthread_mutex_lock(&baguette[left]);
        }
        mange(*id);
        pthread_mutex_unlock(&baguette[left]);
        pthread_mutex_unlock(&baguette[right]);
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Utilisation : %s [Nombre de philosophes]\n", argv[0]);
        return EXIT_FAILURE;
    }

    N = atoi(argv[1]);
    phil = (pthread_t *)malloc(N * sizeof(pthread_t));
    baguette = (pthread_mutex_t *)malloc(N * sizeof(pthread_mutex_t));

    // Initialisation des mutex et création des threads
    for (int i = 0; i < N; ++i) {
        pthread_mutex_init(&baguette[i], NULL);
        int *id = (int *)malloc(sizeof(int));
        *id = i;
        pthread_create(&phil[i], NULL, philosophe, id);
    }

    // Attendre la fin des cycles de pensée/manger
    for (int i = 0; i < N; ++i) {
        pthread_join(phil[i], NULL);
    }

    // Nettoyage et fin du programme
    for (int i = 0; i < N; ++i) {
        pthread_mutex_destroy(&baguette[i]);
    }

    free(phil);
    free(baguette);

    return EXIT_SUCCESS;
}