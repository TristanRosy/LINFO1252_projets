#include "my_lock.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


typedef struct {
    int num_sections;
} ThreadArgs;

void* thread_function(void* arg) {
    int num_sections = *((int*)arg);

    my_lock_t lock = {0}; // Initialisation du verrou par attente active, à l'extérieur de la boucle

    for (int i = 0; i < num_sections; ++i) {
        lock_manager_acquire(&lock, 1); // Utilisation du verrou par attente active

        // Actions dans la section critique

        for (int j = 0; j < 10000; ++j); // Simulation de traitement CPU entre la consommation et la production

        unlock(&lock); // Libérer le verrou après la section critique de production
    }

    pthread_exit(NULL);
}


int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <num_threads> <num_sections>\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);
    int num_sections = atoi(argv[2]);

    pthread_t threads[num_threads];

    for (int i = 0; i < num_threads; ++i) {
        // Allouer et initialiser la structure pour les arguments du thread
        ThreadArgs* args = (ThreadArgs*)malloc(sizeof(ThreadArgs));
        args->num_sections = num_sections;

        // Créer le thread en passant la structure des arguments
        pthread_create(&threads[i], NULL, thread_function, (void*)args);
    }

    // Attendre que tous les threads se terminent
    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}