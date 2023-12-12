#include "my_lock.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


typedef struct {
    int num_sections;
} ThreadArgs;

/*
 * Fonction qui test l'algorithme Test-and-Set.
 */
void* thread_function_tas(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    int num_sections = args->num_sections;

    my_lock_t lock = {0};

    for (int i = 0; i < num_sections; ++i) {
        tas_lock(&lock);

        // Actions dans la section critique

        for (int j = 0; j < 10000; ++j); // Simulation de traitement CPU entre la consommation et la production

        unlock(&lock);
    }

    pthread_exit(NULL);
}

/*
 * Fonction qui test l'algorithme Test-and-Test-and-Set.
 */
void* thread_function_tatas(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    int num_sections = args->num_sections;

    my_lock_t lock = {0};

    for (int i = 0; i < num_sections; ++i) {
        tatas_lock(&lock);

        // Actions dans la section critique

        for (int j = 0; j < 10000; ++j); // Simulation de traitement CPU entre la consommation et la production


        unlock(&lock);
    }

    pthread_exit(NULL);
}

/*
 * Fonction qui test l'algorithme Back-Off Test-and-Test-and-Set.
 */
void* thread_function_bo_tatas(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    int num_sections = args->num_sections;

    my_lock_t lock = {0};

    for (int i = 0; i < num_sections; ++i) {
        bo_tatas_lock(&lock);

        // Actions dans la section critique

        for (int j = 0; j < 10000; ++j); // Simulation de traitement CPU entre la consommation et la production

        unlock(&lock);
    }

    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage: %s <num_threads> <num_sections> <lock_type>\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);
    int num_sections = atoi(argv[2]);
    int lock_type = atoi(argv[3]); // Type de verrou à utiliser

    pthread_t threads[num_threads];

    for (int i = 0; i < num_threads; ++i) {
        // Allouer et initialiser la structure pour les arguments du thread
        ThreadArgs* args = (ThreadArgs*)malloc(sizeof(ThreadArgs));
        args->num_sections = num_sections;

        // Créer le thread en passant la structure des arguments
        switch (lock_type) {
            case 1:
                pthread_create(&threads[i], NULL, thread_function_tas, (void*)args);
                break;
            case 2:
                pthread_create(&threads[i], NULL, thread_function_tatas, (void*)args);
                break;
            case 3:
                pthread_create(&threads[i], NULL, thread_function_bo_tatas, (void*)args);
                break;
            default:
                printf("Invalid lock type\n");
                return 1;
        }
    }

    // Attendre que tous les threads se terminent.
    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
