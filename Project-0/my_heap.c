#include <stdint.h>
#include <stdio.h>

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


void init(){

    // Casting de MY_HEAP en pointeur de uint16_t pour stocker le header initial dans les deux premières cases de la heap.
    uint16_t* ptr = (uint16_t *) &MY_HEAP[0];

    // Si la première case uint16_t est non-nulle càd si la heap stockait déjà des données on la reinitialise entièrement à 0.
    if (*ptr != 0){
        for (int i = 0; i < sizeof(MY_HEAP); ++i) {
            MY_HEAP[i] = 0;
        }
    }

    // On stocke la taille de mémoire libre disponible dans le premier header et le bit de poids faible à 0 pour indiquer que le bloc est libre.
    *ptr = sizeof(MY_HEAP);

}

void my_free(void* pointer){

    // On ne fait rien pointer est nul.
    if (pointer != NULL) {

        // On met le bit de poids faible du header a 0. pour
        uint16_t* h = (uint16_t*)pointer - 1;
        *h &= ~0x1;

        // Si le prochain bloc est libre aussi on additionne les deux tailles.
        uint16_t* next = h + (*h)/2;
        if ((*next & 0x1) == 0) {
            *h += *next;
        }
    }
}


void *my_malloc(size_t size){

    if (size == 0) return NULL;

    size_t heap_len = sizeof(MY_HEAP);

    // Casting de MY_HEAP en pointeur de uint16_t pour lire les headers qui sont de taille uint16_t.
    uint16_t* h = (uint16_t *) &MY_HEAP[0];

    // size + taille du header (2) ajusté avec le facteur d'allignement de 8.
    uint16_t newsize = ((size + 9) / 8) * 8;

    // Parcours les headers jusqu'a trouver un bloc libre de taille suffisante ou bien d'atteindre la fin de la heap (first fit).
    while (((*h & 0x1) != 0 && h < (uint16_t*) &MY_HEAP[heap_len]) || (*h & ~0x1) < newsize){

        // h est incrémenté par la taille stocké dans le header actuel et divisé par deux car notre pointeur est en uint16_t.
        h += (*h & ~0x1)/2;
    }

    // Si un bloc satisfaisant les conditions a été trouvé.
    if (h < (uint16_t *) &MY_HEAP[heap_len]){

        // Si le bloc suivant est libre on ajuste la taille disponible.
        if (*(h + newsize/2)  == 0) {
            *(h + newsize/2) = *h - newsize;
        }

        // On change la taille du header et on met le bit de poids faibla à 1.
        *h = newsize | 0x1;

        // On retourne un pointeur void vers le début de l'espace libre.
        return (void*) h + 2;
    }

    // On retourne NULL si aucun blocs correspondant n'a été trouvé.
    return NULL;
}






