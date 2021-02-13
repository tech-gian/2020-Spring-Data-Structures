#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "ADTPriorityQueue.h"
#include "ADTSet.h"

int* create_int(int value) {
    int* pointer = malloc(sizeof(int)); // δέσμευση μνήμης
    *pointer = value;                   // αντιγραφή του value στον νέο ακέραιο
    return pointer;
}

int compare_ints(Pointer a, Pointer b) {
    return *(int*)a - *(int*)b;
}

int main(void) {
    srand(time(NULL));

    PriorityQueue pq = pqueue_create(compare_ints, free, NULL);
//εισαγωγή 20 τυχαίων αριθμών
    for (int i=0 ; i<20 ; i++) {
        pqueue_insert(pq, create_int(rand()));
    }
//εκτύπωση σε φθίνουσα σειρά
    int pq_size = pqueue_size(pq);
    printf("Pqueue values:\n");
    for (int i=0 ; i<pq_size ; i++) {
        int* value = pqueue_max(pq);
        printf("%d\n", *value);
        pqueue_remove_max(pq);
    }

    pqueue_destroy(pq);

//αντίστοιχα για το set
    Set set = set_create(compare_ints, free);

    for (int i=0 ; i<20 ; i++) {
        set_insert(set, create_int(rand()));
    }

    printf("Set values:\n");
    for (SetNode node=set_first(set);
        node!=SET_EOF;
        node=set_next(set, node)) {
            int* value = set_node_value(set, node);
            printf("%d\n", *value);            
        }

    set_destroy(set);
}