///////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT Priority Queue μέσω σωρού.
//
///////////////////////////////////////////////////////////

#include <stdlib.h>
#include <assert.h>

#include "ADTPriorityQueue.h"
#include "ADTVector.h"			// Η υλοποίηση του PriorityQueue χρησιμοποιεί Vector

// Ενα PriorityQueue είναι pointer σε αυτό το struct
struct priority_queue {
	Vector vector;				// Τα δεδομένα, σε Vector ώστε να έχουμε μεταβλητό μέγεθος χωρίς κόπο
	CompareFunc compare;		// Η διάταξη
	DestroyFunc destroy_value;	// Συνάρτηση που καταστρέφει ένα στοιχείο του vector.
	int steps;					// Πόσα steps έχουν γίνει στην τελευταία συνάρτηση που κλήθηκε
};

struct priority_queue_node {
	Pointer value;				// Η τιμή που αποθηκεύει ένας κόμβος
	int pos;					// Η θέση που βρίσκεται ο κόμβος
};

// Βοηθητικές συναρτήσεις ////////////////////////////////////////////////////////////////////////////

// Προσοχή: στην αναπαράσταση ενός complete binary tree με πίνακα, είναι βολικό τα ids των κόμβων να
// ξεκινάνε από το 1 (ρίζα), το οποίο απλοποιεί τις φόρμουλες για εύρεση πατέρα/παιδιών. Οι θέσεις
// ενός vector όμως ξεκινάνε από το 0. Θα μπορούσαμε απλά να αφήσουμε μία θέση κενή, αλλά δεν είναι ανάγκη,
// μπορούμε απλά να αφαιρούμε 1 όταν διαβάζουμε/γράφουμε στο vector. Για απλοποίηση του κώδικα, η
// πρόσβαση στα στοιχεία του vector γίνεται από τις παρακάτω 2 βοηθητικές συναρτήσεις.

// Επιστρέφει την τιμή του κόμβου node_id

static Pointer node_value(PriorityQueue pqueue, int node_id) {
	// τα node_ids είναι 1-based, το node_id αποθηκεύεται στη θέση node_id - 1
	PriorityQueueNode node = vector_get_at(pqueue->vector, node_id - 1);
	// Επιστροφή της τιμής που περιέχει ο κόμβος
	return node->value;
}

// Ανταλλάσει τις τιμές των κόμβων node_id1 και node_id2

static void node_swap(PriorityQueue pqueue, int node_id1, int node_id2) {
	// τα node_ids είναι 1-based, το node_id αποθηκεύεται στη θέση node_id - 1
	PriorityQueueNode node1 = vector_get_at(pqueue->vector, node_id1 - 1);
	PriorityQueueNode node2 = vector_get_at(pqueue->vector, node_id2 - 1);

	// Αλλαγή της θέσης (pos)
	int temp_pos = node1->pos;
	node1->pos = node2->pos;
	node2->pos = temp_pos;

	// Αλλαγή των nodes
	vector_set_at(pqueue->vector, node_id1 - 1, node2);
	vector_set_at(pqueue->vector, node_id2 - 1, node1);
}

// Αποκαθιστά την ιδιότητα του σωρού.
// Πριν: όλοι οι κόμβοι ικανοποιούν την ιδιότητα του σωρού, εκτός από
//       τον node_id που μπορεί να είναι _μεγαλύτερος_ από τον πατέρα του.
// Μετά: όλοι οι κόμβοι ικανοποιούν την ιδιότητα του σωρού.

static void bubble_up(PriorityQueue pqueue, int node_id) {
	// Αν φτάσαμε στη ρίζα, σταματάμε
	if (node_id == 1)
		return;

	int parent = node_id / 2;		// Ο πατέρας του κόμβου. Τα node_ids είναι 1-based

	// Αν ο πατέρας έχει μικρότερη τιμή από τον κόμβο, swap και συνεχίζουμε αναδρομικά προς τα πάνω
	if (pqueue->compare(node_value(pqueue, parent), node_value(pqueue, node_id)) < 0) {
		node_swap(pqueue, parent, node_id);
		bubble_up(pqueue, parent);
		// Αύξηση ανάλογα πόσες φορές θα κληθεί αναδρομικά
		pqueue->steps++;
	}
}

// Αποκαθιστά την ιδιότητα του σωρού.
// Πριν: όλοι οι κόμβοι ικανοποιούν την ιδιότητα του σωρού, εκτός από τον
//       node_id που μπορεί να είναι _μικρότερος_ από κάποιο από τα παιδιά του.
// Μετά: όλοι οι κόμβοι ικανοποιούν την ιδιότητα του σωρού.

static void bubble_down(PriorityQueue pqueue, int node_id) {
	// βρίσκουμε τα παιδιά του κόμβου (αν δεν υπάρχουν σταματάμε)
	int left_child = 2 * node_id;
	int right_child = left_child + 1;

	int size = pqueue_size(pqueue);
	if (left_child > size)
		return;

	// βρίσκουμε το μέγιστο από τα 2 παιδιά
	int max_child = left_child;
	if (right_child <= size && pqueue->compare(node_value(pqueue, left_child), node_value(pqueue, right_child)) < 0)
			max_child = right_child;

	// Αν ο κόμβος είναι μικρότερος από το μέγιστο παιδί, swap και συνεχίζουμε προς τα κάτω
	if (pqueue->compare(node_value(pqueue, node_id), node_value(pqueue, max_child)) < 0) {
		node_swap(pqueue, node_id, max_child);
		bubble_down(pqueue, max_child);
		// Αύξηση ανάλογα πόσες φορές θα κληθεί αναδρομικά
		pqueue->steps++;
	}
}

// Αρχικοποιεί το σωρό από τα στοιχεία του vector values. (ΜΗ ΑΠΟΔΟΤΙΚΟ)

// static void naive_heapify(PriorityQueue pqueue, Vector values) {
// 	// Απλά κάνουμε insert τα στοιχεία ένα ένα.
// 	// TODO: υπάρχει πιο αποδοτικός τρόπος να γίνει αυτό!
// 	int size = vector_size(values);
// 	for (int i = 0; i < size; i++)
// 		pqueue_insert(pqueue, vector_get_at(values, i));
// }


// Υλοποίηση της παραπάνω συνάρτησης με αποδοτικό τρόπο

static void efficient_heapify(PriorityQueue pqueue, Vector values) {
	int temp_steps = 0;
	int size = vector_size(values);
	// Για κάθε στοιχείο του values
	for (int i=0 ; i<size ; i++) {
		// Δημιουργώ έναν pqueue κόμβο
		PriorityQueueNode node = malloc(sizeof(*node));
		node->value = vector_get_at(values, i);
		node->pos = i + 1;
		
		// και αποθηκεύεται η θέση και το value του
		vector_insert_last(pqueue->vector, node);

		// Προσθέτω τα βήματα που κάνει η vector_insert_last
		temp_steps += vector_steps(pqueue->vector);
	}

	// Από τον τελευταίο γονιό και προς τα πάνω
	// κάνω bubble_down, για να μπουν οι κόμβοι
	// στην σωστή θέση
	for (int i=size/2 ; i>0 ; i--) {
		// Σε κάθε επάναληψη αρχικοποιούμε τα steps
		// και αυξάνονται ανάλογα πόσα bubble_down θα γίνουν
		pqueue->steps = 1;
		bubble_down(pqueue, i);
		temp_steps += pqueue->steps;
	}

	// Βάζουμε το temp_steps στo pqueue->steps
	pqueue->steps = temp_steps;
}


// Συναρτήσεις του ADTPriorityQueue //////////////////////////////////////////////////

PriorityQueue pqueue_create(CompareFunc compare, DestroyFunc destroy_value, Vector values) {
	assert(compare != NULL);	// LCOV_EXCL_LINE

	PriorityQueue pqueue = malloc(sizeof(*pqueue));
	pqueue->compare = compare;
	pqueue->destroy_value = destroy_value;

	// Δημιουργία του vector που αποθηκεύει τα στοιχεία.
	// ΠΡΟΣΟΧΗ: ΔΕΝ περνάμε την destroy_value στο vector!
	// Αν την περάσουμε θα καλείται όταν κάνουμε swap 2 στοιχεία, το οποίο δεν το επιθυμούμε.
	pqueue->vector = vector_create(0, NULL);

	// Αν ο values == NULL τότε θα γίνει 1 βήμα
	// Αλλιώς θα αυξηθεί στην efficient_heapify ανάλογα
	pqueue->steps = 1;

	// Αν values != NULL, αρχικοποιούμε το σωρό.
	if (values != NULL)
		efficient_heapify(pqueue, values);
		// naive_heapify(pqueue, values);

	return pqueue;
}

int pqueue_size(PriorityQueue pqueue) {
	pqueue->steps = 1;
	return vector_size(pqueue->vector);
}

Pointer pqueue_max(PriorityQueue pqueue) {
	pqueue->steps = 1;
	return node_value(pqueue, 1);		// root
}

PriorityQueueNode pqueue_insert(PriorityQueue pqueue, Pointer value) {
	// Δημιουργία κόμβου
	PriorityQueueNode node = malloc(sizeof(*node));
	// Προσθήκη της value και της θέσης του
	node->value = value;
	node->pos = pqueue_size(pqueue) + 1;

	vector_insert_last(pqueue->vector, node);
	// Αποθηκεύουμε τα βήματα της vector_insert_last
	int temp_steps = vector_steps(pqueue->vector);
	
	// Αρχικοποιούμε το steps και θα αυξηθεί ανάλογα πόσα bubble_up θα γίνουν
	pqueue->steps = 1;

 	// Ολοι οι κόμβοι ικανοποιούν την ιδιότητα του σωρού εκτός από τον τελευταίο, που μπορεί να είναι
	// μεγαλύτερος από τον πατέρα του. Αρα μπορούμε να επαναφέρουμε την ιδιότητα του σωρού καλώντας
	// τη bubble_up γα τον τελευταίο κόμβο (του οποίου το 1-based id ισούται με το νέο μέγεθος του σωρού).
	bubble_up(pqueue, pqueue_size(pqueue));

	// Τα συνολικά βήματα που κάνει η pqueue_insert
	pqueue->steps += temp_steps;

	return node;
}

void pqueue_remove_max(PriorityQueue pqueue) {
	int last_node = pqueue_size(pqueue);
	assert(last_node != 0);		// LCOV_EXCL_LINE

	// Αντικαθιστούμε τον πρώτο κόμβο με τον τελευταίο και αφαιρούμε τον τελευταίο
	node_swap(pqueue, 1, last_node);

	// Καταστροφή του value, αν το destroy_value != NULL
	PriorityQueueNode node = vector_get_at(pqueue->vector, last_node-1);
	if (pqueue->destroy_value!=NULL)
		pqueue->destroy_value(node->value);
	// Καταστροφή του ίδιου του κόμβου
	free(node);

	vector_remove_last(pqueue->vector);
	// Αποθηκεύουμε τα βήματα της vector_insert_last
	int temp_steps = vector_steps(pqueue->vector);

	// Αρχικοποιούμε το steps και θα αυξηθεί ανάλογα πόσα bubble_up θα γίνουν
	pqueue->steps = 1;

 	// Ολοι οι κόμβοι ικανοποιούν την ιδιότητα του σωρού εκτός από τη νέα ρίζα
 	// που μπορεί να είναι μικρότερη από κάποιο παιδί της. Αρα μπορούμε να
 	// επαναφέρουμε την ιδιότητα του σωρού καλώντας τη bubble_down για τη ρίζα.
	bubble_down(pqueue, 1);

	// Τα συνολικά βήματα που κάνει η pqueue_insert
	pqueue->steps += temp_steps;
}

DestroyFunc pqueue_set_destroy_value(PriorityQueue pqueue, DestroyFunc destroy_value) {
	pqueue->steps = 1;
	DestroyFunc old = pqueue->destroy_value;
	pqueue->destroy_value = destroy_value;
	return old;
}

void pqueue_destroy(PriorityQueue pqueue) {
	// Καταστροφή του κάθε κόμβου
	int size = pqueue_size(pqueue);
	for (int i=0 ; i<size ; i++) {
		// Καταστροφή του value, αν το destroy_value != NULL
		PriorityQueueNode node = vector_get_at(pqueue->vector, i);
		if (pqueue->destroy_value!=NULL)
			pqueue->destroy_value(node->value);
		// Καταστροφή του ίδιου του κόμβου
		free(node);
	}
	
	// Καταστροφή το υπόλοιπο vector
	vector_destroy(pqueue->vector);
	// Καταστροφή του ίδιου του PriorityQueue
	free(pqueue);
}



//// Νέες συναρτήσεις για την εργασία 2 //////////////////////////////////////////

Pointer pqueue_node_value(PriorityQueue pqueue, PriorityQueueNode node) {
	pqueue->steps = 1;
	// Επιστροφή της τιμής του συγκεκριμένου κόμβου
	return node->value;
}

void pqueue_remove_node(PriorityQueue pqueue, PriorityQueueNode node) {
	int cur = node->pos;
	// Αλλαγή του κόμβου με τον τελευταίο
	node_swap(pqueue, pqueue_size(pqueue), node->pos);

	// Καταστροφή του value, αν το destroy_value != NULL
	if (pqueue->destroy_value!=NULL)
			pqueue->destroy_value(node->value);
	// Καταστροφή του ίδιου του κόμβου
	free(node);

	// Καταστροφή του κόμβου του vector
	vector_remove_last(pqueue->vector);
	// Αποθηκεύουμε τα βήματα της vector_insert_last
	int temp_steps = vector_steps(pqueue->vector);
	
	// Αρχικοποιούμε το steps και θα αυξηθεί ανάλογα πόσα bubble_up θα γίνουν
	pqueue->steps = 1;
	
	// Για να μπει ο κόμβος (ο παλιός τελευταίος του σωρού) 
	// στην σωστή θέση
	bubble_down(pqueue, cur);

	pqueue->steps += temp_steps;
}

void pqueue_update_order(PriorityQueue pqueue, PriorityQueueNode node) {
	// Αρχικοποιούμε το steps και αυξάνεται ανάλογα σε ποιο,
	// ή κανένα, από τα bubble_up και bubble_down μπει.
	// Μετά θα αυξηθεί μέσα στην αντίστοιχη συνάρτηση
	pqueue->steps = 1;

	// Κάνω bubble_up αν το value μεγάλωσε
	// και bubble_down αν το value μίκρυνε,
	// σε σχέση με το αρχικό
	bubble_up(pqueue, node->pos);
	bubble_down(pqueue, node->pos);
	
	// Ένα από τα δύο δεν θα κάνει καμία επανάληψη
	// και αν το νέο value δεν επηρεάζει την διάταξη,
	// τότε θα γίνει return και από τις δύο, χωρίς
	// καμία αναδρομή
}

// Bonus της Εργασίας - Άσκηση 1

int pqueue_steps(PriorityQueue pqueue) {
	// Απλά επιστρέφουμε τον αριθμό των steps
	// που έκανε η τελευταία καλούσα λειτουργία
	return pqueue->steps;
}
