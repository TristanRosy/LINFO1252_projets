#include <stdint.h>

uint8_t MY_HEAP[64000];

/*
 * Instructions nécessaires pour préparer MY_HEAP avant l'exécution de vos méthodes malloc/free.
 * Cette fonction sera appelée avant chaque nouveau test de votre implémentation.
 * Elle doit donc préparer MY_HEAP dans un état initial où aucune allocation n'a encore eu lieu.
 */
void init();

/*
 * Elle libère une zone mémoire précédemment réservée indiquée par le pointeur ptr.
 * Pour plus de détails concernant ces fonctions
 */
void my_free(void *pointer);

/*
 * Elle renvoie un pointeur vers une zone de mémoire d'une taille minimum de size octets issue de ce tableau.
 * La zone mémoire commençant à l’adresse à laquelle renvoie le pointeur doit rester utilisable
 * dans son entièreté pendant toute la durée de l’utilisation de celui-ci, càd jusqu’à sa libération avec la
 * fonction free.
 */
void *my_malloc(size_t size);







