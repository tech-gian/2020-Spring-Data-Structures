////////////////////////////////////////////////
// 
// Υλοποίηση του ADT Priority Queue μέσω ADT Set
// 
////////////////////////////////////////////////

#include <stdlib.h>
#include <assert.h>

#include "ADTSet.h"
#include "ADTPriorityQueue.h"


struct priority_queue {
    Set set;    // Ουσιαστικά το priority_queue είναι ένα set
};

typedef struct set_node priority_queue_node;

// Συμπληρωματική συνάρτηση
// Αρχικοποιεί το Set από τα στοιχεία του Vector

static void efficient_heapify(PriorityQueue pqueue, Vector values) {
    // Απλά γίνονται insert τα στοιχεία ένα-ένα
    int size = vector_size(values);
    for (int i=0 ; i<size ; i++) {
        pqueue_insert(pqueue, vector_get_at(values, i));
    }
}

// Κύριες συναρτήσεις

PriorityQueue pqueue_create(CompareFunc compare, DestroyFunc destroy_value, Vector values) {
    assert(compare!=NULL);

    // Δέσμευση χώρου για το priority_queue και το περιεχόμενό του
    PriorityQueue pqueue = malloc(sizeof(*pqueue));
    pqueue->set = set_create(compare, destroy_value);

    // Αν υπάρχει initial data, το περνάμε στο set
    if (values!=NULL)
        efficient_heapify(pqueue, values);

    return pqueue;
}


int pqueue_size(PriorityQueue pqueue) {
    return set_size(pqueue->set);
}

Pointer pqueue_max(PriorityQueue pqueue) {
    return set_node_value(pqueue->set, set_last(pqueue->set));
}

PriorityQueueNode pqueue_insert(PriorityQueue pqueue, Pointer value) {
    // Εισαγωγή του value στην pqueue και επιστροφή του κόμβου
    set_insert(pqueue->set, value);
    return (PriorityQueueNode) set_find_node(pqueue->set, value);
}

void pqueue_remove_max(PriorityQueue pqueue) {
    set_remove(pqueue->set, set_node_value(pqueue->set, set_last(pqueue->set)));
}

DestroyFunc pqueue_set_destroy_value(PriorityQueue pqueue, DestroyFunc destroy_value) {
    // Γυρίζω την παλιά destroy_value και βάζω την νέα στο Set
    return set_set_destroy_value(pqueue->set, destroy_value);
}

void pqueue_destroy(PriorityQueue pqueue) {
    // Αποδέσμευση του Set
    set_destroy(pqueue->set);

    // Αποδέσμευση του ίδιου του priority_queue
    free(pqueue);
}


// Νέες συναρτήσεις για την εργασία 2 ////////////////////////////

Pointer pqueue_node_value(PriorityQueue pqueue, PriorityQueueNode node) {
    // Επιστροφή του περιεχομένου του node από το Set
    return set_node_value(pqueue->set, (SetNode) node);
}

void pqueue_remove_node(PriorityQueue pqueue, PriorityQueueNode node) {
    // Αφαίρεση του node από το Set
    set_remove(pqueue->set, set_node_value(pqueue->set, (SetNode) node));
}

void pqueue_update_order(PriorityQueue pqueue, PriorityQueueNode node) {
    // Δεν χρειάζεται να υλοποιηθεί για την άσκηση 3
}
