#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include "my_lock.h"
#include "my_sem.h"

#define BUFFER_SIZE 8 // Taille du buffer
#define DATA_SIZE 8192 // Nombre de données à produire et consommer

int buffer[BUFFER_SIZE];

int in; // Indice de production
int out; // Indice de consommation
int count; // Nombre d'éléments dans le buffer

my_lock_t mutex; // Mutex pour protéger l'accès au buffer et aux variables in, out et count.
my_sem_t empty; // Sémaphore qui indique le nombre de places libres dans le buffer.
my_sem_t full;  // Sémaphore qui indique le nombre de places remplies dans le buffer.

/*
 * Fonction appelée en cas d'erreur.
 */
void error(int err, char *msg, int index){
    fprintf(stderr, "%s %d a retourné %d, message d'erreur : %s\n", msg,index, err, strerror(errno));
    exit(EXIT_FAILURE);
}

/*
 * Fonction producteur, génère un entier entre MIN_INT et MAX_INT
 * et le met dans une place libre du buffer.
 */
void *producer(void* arg){

    int item;
    int *nb_prod = (int*) arg;

    for (int i = 0; i < *nb_prod; i++){

        item = rand() - RAND_MAX / 2; // Production d'un entier entre MIN_INT et MAX_INT.

        my_sem_wait(&empty); // Attend qu'une place sois libre dans le buffer.
        tatas_lock(&mutex);

        /// Section critique
        buffer[in] = item;
        in = (in + 1) % BUFFER_SIZE;
        count++;
        /// Section critique

        unlock(&mutex);
        my_sem_post(&full); // Indique qu'une place supplémentaire est remplie dans le buffer.

        for (int j = 0; j < 10000; j++); // simule un traitement utilisant de la ressource CPU
    }

    pthread_exit(NULL);
}

/*
 * Fonction consommateur, marque la case consommée comme libre.
 */
void *consumer(void* arg){

    int *nb_cons = (int *) arg;

    for (int i = 0; i < *nb_cons; i++){

        my_sem_wait(&full); // Attend qu'une place sois remplie dans le buffer.
        tatas_lock(&mutex);

        /// Section critique
        //printf("Consumed : %d\n", buffer[out]); // Consommation
        out = (out + 1) % BUFFER_SIZE;
        count--;
        /// Section critique

        unlock(&mutex);
        my_sem_post(&empty); // Indique qu'une place supplémentaire est libre dans le buffer.

        for (int i = 0; i < 10000; i++); // simule un traitement utilisant de la ressource CPU
    }

    pthread_exit(NULL);
}

int main(int argc, char* argv[]){

    // Lecture des deux premiers argument du programme.
    int nb_cons = atoi(argv[1]);
    int nb_prod = atoi(argv[2]);

    // Nombre de productions et consommations par threads.
    int nb_prod_per_thread = DATA_SIZE / nb_prod;
    int nb_cons_per_thread = DATA_SIZE / nb_cons;

    pthread_t producers[nb_prod];
    pthread_t consumers[nb_cons];

    // Initialisations des indices.
    in = 0;
    out = 0;
    count = 0;

    // Initialisation du mutex et des sémaphores.
    my_lock_init(&mutex);
    my_sem_init(&empty, BUFFER_SIZE);
    my_sem_init(&full, 0);

    // initialisation pour que rand() fonctionne correctement.
    srand(time(NULL));

    int err;

    for (int i = 0; i < nb_prod; ++i) {
        if (i == nb_prod - 1) nb_prod_per_thread += DATA_SIZE % nb_prod; // Afin de permettre tout nombre de producteurs.

        err = pthread_create(&producers[i], NULL,  &producer, &nb_prod_per_thread);
        if (err != 0) error(err, "pthread_create producer :", i);
    }

    for (int i = 0; i < nb_cons; ++i) {
        if (i == nb_cons - 1) nb_cons_per_thread += DATA_SIZE % nb_cons; // Afin de permettre tout nombre de consommateurs.

        err = pthread_create(&consumers[i], NULL,  &consumer, &nb_cons_per_thread);
        if (err != 0) error(err, "pthread_create consumer :", i);
    }

    for (int i = 0; i < nb_prod; i++) {
        err = pthread_join(producers[i], NULL);
        if (err != 0) error(err, "pthread_join producer :", i);
    }

    for (int i = 0; i < nb_cons; i++) {
        err = pthread_join(consumers[i], NULL);
        if (err != 0) error(err, "pthread_join consumer :", i);
    }

    //printf("nb_cons = %d, nb_prod = %d : execution finished!\n", nb_cons, nb_prod);

    return EXIT_SUCCESS;
}

