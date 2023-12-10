#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "my_lock.h"
#include "my_sem.h"

#define N_WRITINGS 640
#define N_READINGS 2560

my_lock_t mutex_writer; // Mutex pour protéger la variable write_count.
my_lock_t mutex_reader; // Mutex pour protéger la variable read_count.
my_lock_t z; // Mutex permettant de donner la priorité absolue aux écrivains.
my_sem_t db_writer; // Bloquer et débloquer l'accès aux écrivains.
my_sem_t db_reader; // Bloquer et débloquer l'accès aux lecteurs.

int write_count; // Compte le nombre d'écritures.
int read_count; // Compte le nombre de lectures.

/*
 * Fonction appelé en cas d'erreur.
 */
void error(int err, char *msg, int index){
    fprintf(stderr, "%s %d a retourné %d, message d'erreur : %s\n", msg,index, err, strerror(errno));
    exit(EXIT_FAILURE);
}

/*
 * Fonction écrivain
 */
void *writer(void *arg) {

    int *nb_writings = (int *) arg;

    for (int i = 0; i < *nb_writings; i++){

        tas_lock(&mutex_writer);

        /// section critique
        write_count++;
        if (write_count == 1) { // Arrivée du premier reader.
            my_sem_wait(&db_reader); // Bloque les lecteurs.
        }
        /// section critique

        unlock(&mutex_writer);
        my_sem_wait(&db_writer); // Bloque les autres écrivains et vérifie que la database n'est pas en train d'être lue.

        for (int j = 0; j < 10000; ++j); // Simule l'écriture.

        my_sem_post(&db_writer); // Libère les autres écrivains.

        tas_lock(&mutex_writer);

        /// section critique
        write_count--;
        if (write_count == 0) { // Départ du dernier writer
            my_sem_post(&db_reader); // Libère les lecteurs
        }
        /// section critique

        unlock(&mutex_writer);
    }

    pthread_exit(NULL);
}

/*
 * Fonction lecteur
 */
void *reader(void *arg) {

    int *nb_readings = (int *) arg;

    for (int i = 0; i < *nb_readings; i++){

        tas_lock(&z);
        my_sem_wait(&db_reader); // Attente d'être débloquer par les écrivains.
        tas_lock(&mutex_reader);

        /// section critique
        read_count++;
        if (read_count == 1) { // Arrivée du premier lecteur.
            my_sem_wait(&db_writer); // Premier lecteur réserve la database pour être sur qu'aucun écrivain ne l'utilise.
        }
        /// section critique

        unlock(&mutex_reader);
        my_sem_post(&db_reader);
        unlock(&z);

        for (int j = 0; j < 10000; ++j); // Simule la lecture.

        tas_lock(&mutex_reader);

        /// section critique
        read_count--;
        if (read_count == 0) { // Départ du dernier lecteur de la database écrivain.
            my_sem_post(&db_writer);
        }
        /// section critique

        unlock(&mutex_reader);
    }

    pthread_exit(NULL);

}

int main(int argc, char* argv[]){

    // Lecture des deux premiers argument du programme.
    int nb_reader = atoi(argv[1]);
    int nb_writer = atoi(argv[2]);

    // Nombre d'écritures et lectures par threads.
    int nb_readings_per_thread = N_READINGS / nb_reader;
    int nb_writings_per_thread= N_WRITINGS / nb_writer;

    pthread_t readers[nb_reader];
    pthread_t writers[nb_writer];

    // Initialisation des variables qui comptent.
    read_count = 0;
    write_count = 0;

    // Initialisation des mutex et des sémaphores.
    my_lock_init(&mutex_reader);
    my_lock_init(&mutex_writer);
    my_lock_init(&z);
    my_sem_init(&db_writer, 1);
    my_sem_init(&db_reader, 1);

    int err;

    for (int i = 0; i < nb_writer; ++i) {
        if (i == nb_writer - 1) nb_writings_per_thread += N_WRITINGS % nb_writer; // Afin de permettre tout nombre d'écrivains.

        err = pthread_create(&writers[i], NULL,  &writer, &nb_writings_per_thread);
        if (err != 0) error(err, "pthread_create producer :", i);
    }

    for (int i = 0; i < nb_reader; ++i) {
        if (i == nb_reader - 1) nb_readings_per_thread += N_READINGS % nb_reader; // Afin de permettre tout nombre de lecteurs.

        err = pthread_create(&readers[i], NULL,  &reader, &nb_readings_per_thread);
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

    //printf("Writers = %d, Readers = %d : execution finished!\n", nb_writer, nb_reader);

    return EXIT_SUCCESS;
}