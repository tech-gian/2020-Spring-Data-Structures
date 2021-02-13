	///////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT Priority Queue μέσω ταξινομημένης λίστας.
//
///////////////////////////////////////////////////////////

#include <stdlib.h>
#include <assert.h>

#include "ADTPriorityQueue.h"
#include "ADTList.h"			// Η υλοποίηση του PriorityQueue χρησιμοποιεί List


// Ενα PriorityQueue είναι pointer σε αυτό το struct
struct priority_queue {
	List list;					// Η λίστα στην οποία αποθηκεύουμε τα στοιχεία
	CompareFunc compare;		// Η διάταξη
	DestroyFunc destroy_value;	// Συνάρτηση που καταστρέφει κάθε στοιχείο της λίστας
};

// Αρχικοποιεί την λίστα με τις τιμές του values

static void initialize(PriorityQueue pqueue, Vector values) {
	int size = vector_size(values);
	
	// Κάθε στοιχείο του vector, το περνάμε στην list
	for (int i=0 ; i<size ; i++) {
		pqueue_insert(pqueue, vector_get_at(values, i));
	}
}


// Συναρτήσεις του ADTPriorityQueue //////////////////////////////////////////////////

PriorityQueue pqueue_create(CompareFunc compare, DestroyFunc destroy_value, Vector values) {
	assert(compare != NULL);	// LCOV_EXCL_LINE

	PriorityQueue pqueue = malloc(sizeof(*pqueue));
	pqueue->compare = compare;
	pqueue->destroy_value = destroy_value;

	// Για την κλήση της συνάρτησης destroy_value, την περνάμε απλά στη list_create
	// οπότε θα καλείται αυτόματα όταν το στοιχείο αφαιρεθεί από τη λίστα.
	pqueue->list = list_create(destroy_value);

	// Αν values!=NULL, αρχικοποιούμε την λίστα
	if (values!=NULL)
		initialize(pqueue, values);

	return pqueue;
}

int pqueue_size(PriorityQueue pqueue) {
	return list_size(pqueue->list);
}

Pointer pqueue_max(PriorityQueue pqueue) {
	return list_node_value(pqueue->list, list_first(pqueue->list));
}

void pqueue_insert(PriorityQueue pqueue, Pointer value) {
	// Αν η λίστα είναι κενή
	if (list_size(pqueue->list)==0) {
		list_insert_next(pqueue->list, LIST_BOF, value);
	}
	// Αν το στοιχείο είναι μεγαλύτερο από το πρώτο (το μεγαλύτερο)
	else if (pqueue->compare(list_node_value(pqueue->list, list_first(pqueue->list)), value) < 0) {
		list_insert_next(pqueue->list, LIST_BOF, value);
	}
	// Αν το στοιχείο είναι μικρότερο από το τελευταίο (το μικρότερο)
	else if (pqueue->compare(list_node_value(pqueue->list, list_last(pqueue->list)), value) > 0) {
		list_insert_next(pqueue->list, list_last(pqueue->list), value);
	}
	// Σε όλες τις άλλες περιπτώσεις
	else {
		for (ListNode node=list_first(pqueue->list);
			node!=list_last(pqueue->list);
			node=list_next(pqueue->list, node)) {

				if (pqueue->compare(list_node_value(pqueue->list, list_next(pqueue->list, node)), value) < 0) {
					list_insert_next(pqueue->list, node, value);
					break;
				}
			}
	}

	// Υποσημείωση: αν το στοιχείο είναι μικρότερο από το πρώτο
	// και δεν υπάρχει άλλο στοιχείο στην λίστα, τότε δεν γίνεται
	// το loop στο else. Αλλά το στοιχείο έχει μπει στην σωστή θέση,
	// αφού το list_last συμπίπτει με list_first
}

void pqueue_remove_max(PriorityQueue pqueue) {
	// Απλά αφαιρούμε τον πρώτο κόμβο
	list_remove_next(pqueue->list, LIST_BOF);
}

DestroyFunc pqueue_set_destroy_value(PriorityQueue pqueue, DestroyFunc destroy_value) {
	// Απλά καλούμε την αντίστοιχη συνάρτηση της λίστας
	return list_set_destroy_value(pqueue->list, destroy_value);
}

void pqueue_destroy(PriorityQueue pqueue) {
	// Καταστρέφουμε όλη τη λίστα, με τα στοιχεία της
	list_destroy(pqueue->list);

	free(pqueue);
}
