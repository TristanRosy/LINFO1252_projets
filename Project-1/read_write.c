#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define N_WRITINGS 640
#define N_READINGS 2560

pthread_mutex_t mutex_writer;
pthread_mutex_t z;
pthread_mutex_t mutex_reader;

sem_t db_writer;
sem_t db_reader;

int read_count;
int write_count;


/*
 * Fonction appelé en cas d'erreur.
 */
void error(int err, char *msg, int index){
    fprintf(stderr, "%s %d a retourné %d, message d'erreur : %s\n", msg,index, err, strerror(errno));
    exit(EXIT_FAILURE);
}


void *writer(void *arg) {

    int *nb_writings = (int *) arg;

    for (int i = 0; i < *nb_writings; i++){
        pthread_mutex_lock(&mutex_writer);  // mutex pr protéger la variable write_count

        /// section critique
        write_count++;
        if (write_count == 1) { // arrivée du premier reader
            sem_wait(&db_reader); // bloque les lecteurs
        }
        pthread_mutex_unlock(&mutex_writer);
        sem_wait(&db_writer); // bloque les autres writers et vérifie que la database n'est pas en train d'être lue
        /// section critique : un seul writer à la fois
        /// write database()


        for (int j = 0; j < 10000; ++j);

        sem_post(&db_writer); // libère les autres writers

        /// section critique
        pthread_mutex_lock(&mutex_writer);
        write_count--;
        if (write_count == 0) { // départ du dernier writer
            sem_post(&db_reader); // libère les lecteurs
        }
        pthread_mutex_unlock(&mutex_writer);
        /// section critique
    }

    pthread_exit(NULL);
}

void *reader(void *arg) {

    int *nb_readings = (int *) arg;

    for (int i = 0; i < *nb_readings; i++){

        /// section critique
        pthread_mutex_lock(&z); // permet de donner la priorité absolue au writers
        sem_wait(&db_reader); // Sémaphore permetant aux écrivains de bloquer les lecteurs
        pthread_mutex_lock(&mutex_reader); // mutex pour protéger la variable read_count
        read_count++;
        if (read_count == 1) { // arrivée du premier reader
            sem_wait(&db_writer); // premier reader réserve la database pr être sur qu'aucun writer ne l'utilise
        }
        pthread_mutex_unlock(&mutex_reader);
        sem_post(&db_reader);
        pthread_mutex_unlock(&z);
        /// section critique

        for (int j = 0; j < 10000; ++j);

        /// section critique
        pthread_mutex_lock(&mutex_reader);
        read_count--;
        if (read_count == 0) { // départ du dernier reader de la database écrivain
            sem_post(&db_writer);
        }
        pthread_mutex_unlock(&mutex_reader);
        /// section critique
    }

    pthread_exit(NULL);

}

int main(int argc, char* argv[]){

    int nb_reader = atoi(argv[1]);
    int nb_writer = atoi(argv[2]);

    int nb_readings_per_reader = N_READINGS / nb_reader;
    int nb_writings_per_reader = N_WRITINGS / nb_writer;

    pthread_t readers[nb_reader];
    pthread_t writers[nb_writer];

    read_count = 0;
    write_count = 0;

    pthread_mutex_init(&mutex_reader, NULL);
    pthread_mutex_init(&mutex_writer, NULL);
    pthread_mutex_init(&z, NULL);
    sem_init(&db_writer, 0, 1);
    sem_init(&db_reader, 0, 1);

    int err;

    for (int i = 0; i < nb_writer; ++i) {
        err = pthread_create(&writers[i], NULL,  &writer, &nb_writings_per_reader);
        if (err != 0) error(err, "pthread_create producer :", i);
    }

    for (int i = 0; i < nb_reader; ++i) {
        err = pthread_create(&readers[i], NULL,  &reader, &nb_readings_per_reader);
        if (err != 0) error(err, "pthread_create consumer :", i);
    }

    for (int i = 0; i < nb_writer; i++) {
        err = pthread_join(writers[i], NULL);
        if (err != 0) error(err, "pthread_join producer :", i);
    }

    for (int i = 0; i < nb_reader; i++) {
        err = pthread_join(readers[i], NULL);
        if (err != 0) error(err, "pthread_join consumer :", i);
    }

    // Destruction du mutex et des sémaphores.
    pthread_mutex_destroy(&mutex_reader);
    pthread_mutex_destroy(&mutex_writer);
    pthread_mutex_destroy(&z);
    sem_destroy(&db_reader);
    sem_destroy(&db_writer);

    printf("Writers = %d, Readers = %d : execution finished!\n", nb_writer, nb_reader);

    return EXIT_SUCCESS;
}