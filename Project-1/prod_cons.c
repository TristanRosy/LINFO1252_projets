#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#define BUFFER_SIZE 8 // Taille du buffer
#define DATA_SIZE 8192 // Données produites et consommées

int buffer[BUFFER_SIZE];

int in; // Indice de production
int out; // Indice de consommation
int count; // Nombre d'éléments dans le buffer

pthread_mutex_t mutex;
sem_t empty;
sem_t full;

/*
 * Fonction appelé en cas d'erreur.
 */
void error(int err, char *msg, int index){
    fprintf(stderr, "%s %d a retourné %d, message d'erreur : %s\n", msg,index, err, strerror(errno));
    exit(EXIT_FAILURE);
}

/*
 * Fonction producteur, génère un entier entre MIN_INT et MAX_INT et le place dans la première case libre du buffer.
 */
void *producer(void* arg){

    int item;
    int *nb_prod = (int *) arg;

    for (int i = 0; i < *nb_prod; i++){

        item = rand();

        sem_wait(&empty);
        pthread_mutex_lock(&mutex);

        /// Section critique
        buffer[in] = item;
        in = (in + 1) % BUFFER_SIZE;
        count++;
        /// Section critique

        pthread_mutex_unlock(&mutex);
        sem_post(&full);

        for (int i = 0; i < 10000; i++); // simule un traitement utilisant de la ressource CPU
    }

    pthread_exit(NULL);
}

/*
 * Fonction consommateur, marque la case consommée comme libre.
 */
void *consumer(void* arg){

    int *nb_cons = (int *) arg;

    for (int i = 0; i < *nb_cons; i++){

        sem_wait(&full);
        pthread_mutex_lock(&mutex);

        /// Section critique
        out = (out + 1) % BUFFER_SIZE;
        count--;
        /// Section critique

        pthread_mutex_unlock(&mutex);
        sem_post(&empty);

        for (int i = 0; i < 10000; i++); // simule un traitement utilisant de la ressource CPU
    }

    pthread_exit(NULL);
}

int main(int argc, char* argv[]){

    // Lecture des deux premiers argument du programmes.
    int nb_cons = atoi(argv[1]);
    int nb_prod = atoi(argv[2]);

    int nb_prod_per_prod = DATA_SIZE / nb_prod;
    int nb_cons_per_cons = DATA_SIZE / nb_cons;

    pthread_t producers[nb_prod];
    pthread_t consumers[nb_cons];

    in = 0;
    out = 0;
    count = 0;

    // Initialisation du mutex et des sémaphores.
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);

    // initialisation pour que rand() fonctionne correctement (c'est pas exactement ça mais osef).
    srand(time(NULL));

    int err;

    for (int i = 0; i < nb_prod; ++i) {
        err = pthread_create(&producers[i], NULL,  &producer, &nb_prod_per_prod);
        if (err != 0) error(err, "pthread_create producer :", i);
    }

    for (int i = 0; i < nb_cons; ++i) {
        err = pthread_create(&consumers[i], NULL,  &consumer, &nb_cons_per_cons);
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

    // Destruction du mutex et des sémaphores.
    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    printf("nb_cons = %d, nb_prod = %d : execution finished!\n", nb_cons, nb_prod);

    return EXIT_SUCCESS;
}

