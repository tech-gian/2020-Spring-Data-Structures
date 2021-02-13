////////////////////////////////
//
// Υλοποίηση του pq_sort module
//
////////////////////////////////

#include <stdlib.h>

#include "pq_sort.h"
#include "ADTPriorityQueue.h"
#include <stdio.h>

int* create_int(int value) {
	int* pointer = malloc(sizeof(int));	// δέσμευση μνήμης
	*pointer = value;					// αντιγραφή του value στον νέο ακέραιο
	return pointer;
}


void pq_sort_vector(Vector vec, CompareFunc compare) {
	// Προσοχή! Μέσα στη συνάρτηση αυτή θα χρειαστεί να αντικαταστήσουμε τα περιεχόμενα του vector. Δε
	// θέλουμε όμως όσο το κάνουμε αυτό το vector να καλέσει τη destroy! Οπότε αλλάζουμε προσωρινά τη
	// συνάρτηση destroy σε NULL (αποθηκεύοντας την παλιά τιμή).
	DestroyFunc old_destroy = vector_set_destroy_value(vec, NULL);
	PriorityQueue pq_vec = pqueue_create(compare, free, vec);

	// Βάζουμε στο vector το max από το priority queue
	int pq_size = pqueue_size(pq_vec);
	for (int i=0 ; i<pq_size ; i++) {
		int* value = pqueue_max(pq_vec);
		vector_set_at(vec, vector_size(vec)-i-1, create_int(*value));
		pqueue_remove_max(pq_vec);
	}

	pqueue_destroy(pq_vec);

	// Επαναφορά της destroy
	vector_set_destroy_value(vec, old_destroy);
}



void pq_sort_list(List list, CompareFunc compare) {
	// Κρατάμε την παλιά DestroyFunc και την αλλάζουμε σε NULL
	DestroyFunc old_destroy = list_set_destroy_value(list, free);
	PriorityQueue pq_list = pqueue_create(compare, free, NULL);

	// Βάζουμε τις τιμές της λίστας στο priority queue
	for (ListNode node=list_first(list);	
		node!=LIST_EOF;
		node=list_next(list, node)) {
			int* value = list_node_value(list, node);
			pqueue_insert(pq_list, create_int(*value));
		}

	// Άδειασμα των κόμβων της λίστας
	int l_size = list_size(list);
	for (int i=0 ; i<l_size ; i++) {
		list_remove_next(list, LIST_BOF);
	}

	// Ξαναβάζουμε στην λίστα τα στοιχεία με την σωστή σειρά
	int pq_size = pqueue_size(pq_list);
	for (int i=0 ; i<pq_size ; i++) {
		int* value = pqueue_max(pq_list);
		list_insert_next(list, LIST_BOF, create_int(*value));
		pqueue_remove_max(pq_list);
	}

	pqueue_destroy(pq_list);

	// Επαναφορά της destroy
	list_set_destroy_value(list, old_destroy);
}
