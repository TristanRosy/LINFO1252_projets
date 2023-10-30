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

    // Si la première case uint16_t est non-nulle càd si la heap stockait déjà des données on la reinitialise toutes les cases à 0.
    if (*ptr != 0){
        for (int i = 0; i < sizeof(MY_HEAP); ++i) {
            MY_HEAP[i] = 0;
        }
    }

    // On stocke la taille de mémoire libre disponible dans le premier header et le bit de poids faible à 0 pour indiquer que le bloc est libre.
    *ptr = sizeof(MY_HEAP);

}

void my_free(void* pointer){

    // On ne fait rien si pointer est nul.
    if (pointer != NULL) {

        // On met le bit de poids faible du header a 0.
        uint16_t* h = (uint16_t*)pointer - 1;
        *h &= ~0x1;

        // On fusionne les bloc suivant tant qu'ils sont libres et on additionne les tailles.
        uint16_t* next = h + (*h)/2;
        while ((*next & 0x1) == 0 && next < (uint16_t*) &MY_HEAP[sizeof(MY_HEAP)]) {
            uint16_t temp = *next;
            *next = 0; // On efface le header.
            *h += temp;
            next += temp/2;

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

    // Parcours les headers jusqu'a trouver un bloc libre de taille suffisante (first fit) ou bien d'atteindre la fin de la heap.
    while (h < (uint16_t*) &MY_HEAP[heap_len]){

        if ((*h & 0x1) == 0){

            // On fusionne les bloc suivant tant qu'ils sont libres et on additionne les tailles (comme dans my_free).
            uint16_t* next = h + (*h)/2;
            while ((*next & 0x1) == 0 && next < (uint16_t*) &MY_HEAP[heap_len]){
                uint16_t temp = *next;
                *next = 0;
                *h += temp;
                next += temp/2;
            }

            // Si le bloc libre satisfait les conditions.
            if (*h >= newsize){

                // Si la place disponible est supérieur à la place demandée, on ajuste les tailles.
                if (*h > newsize){
                    *(h + newsize/2) = *h - newsize;
                    *h = newsize;
                }

                // On met le bit de poids faible à 1.
                *h |= 0x1;

                // On retourne un pointeur void vers le début de l'espace libre.
                return (void*) h + 2;

            } else {

                // Si la taille du bloc libre n'est pas suffisante, h est incrémenté.
                h += (*h)/2;
            }
        } else {

            // Si le bloc est occupé, h est incrémenté.
            h += (*h & ~0x1)/2;
        }
    }

    // On retourne NULL si aucun blocs correspondant n'a été trouvé.
    return NULL;
}






