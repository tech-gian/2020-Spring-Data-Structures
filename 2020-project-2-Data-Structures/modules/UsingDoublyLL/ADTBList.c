///////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT BList μέσω διπλά συνδεδεμένης λίστας.
//
///////////////////////////////////////////////////////////

#include <stdlib.h>
#include <assert.h>

#include "ADTBList.h"


// Ενα BList είναι pointer σε αυτό το struct
struct blist {
	BListNode dummy;			// Χρησιμοποιείται dummy κόμβος, ώστε και μια κενή λίστα να έχει έναν κόμβο
	BListNode first;			// Δείκτης στον πρώτο κόμβο, ή στον dummy (αν η λίστα είναι κενή)
	int size;					// Μέγεθος, ώστε η blist_size να ειναι O(1)
	DestroyFunc destroy_value;	// Συνάρτηση που καταστρέφει ένα στοιχείο της λίστας
};

struct blist_node {
	BListNode previous;			// Δείκτης στον προηγούμενο
	BListNode next;				// Δείκτης στον επόμενο
	Pointer value;				// Η τιμή που αποθηκεύεται στον κόμβο
};


BList blist_create(DestroyFunc destroy_value) {
	// Δημιουργία του struct
	BList blist = malloc(sizeof(*blist));
	blist->size = 0;
	blist->destroy_value = destroy_value;
	
	// Δημιουργία του dummy κόμβου
	blist->dummy = malloc(sizeof(*blist->dummy));
	// Σε άδεια λίστα, ο dummy δεν έχει ούτε επόμενο ούτε προηγούμενο
	blist->dummy->next = NULL;
	blist->dummy->previous = NULL;
	
	// Σε μια κενή λίστα, ο πρώτος κόμβος είναι επίσης ο dummy
	blist->first = blist->dummy;

	return blist;
}

int blist_size(BList blist) {
	return blist->size;
}

void blist_insert(BList blist, BListNode node, Pointer value) {
	// Αν το node ειναι NULL, απλά εισάγουμε πριν τον dummmy
	if (node==NULL)
		node = blist->dummy;

	// Δημιουργία του νέου κόμβου
	BListNode new = malloc(sizeof(*new));
	new->value = value;

	// Σύνδεση του new ανάμεσα στον προηγούμενο και τον επόμενο
	new->previous = node->previous;
	new->next = node;
	node->previous = new;
	if (new->previous!=NULL)
		(new->previous)->next = new;

	// Ενημέρωση του size και του first
	blist->size++;
	if (blist->first==node)
		blist->first = new;
}

void blist_remove(BList blist, BListNode node) {
	// Ο κόμβος πρέπει να υπάρχει στην λίστα για να διαγραφεί,
	// άρα δεν χρειάζεται να γίνει έλεγχος για node==NULL
	
	if (blist->destroy_value!=NULL)
		blist->destroy_value(node->value);

	// Σύνδεση του επόμενου με τον προηγούμενο
	(node->next)->previous = node->previous;
	// Σύνδεση του προηγούμενου με τον επόμενο (εφόσον υπάρχει)
	if (node->previous!=NULL)
		(node->previous)->next = node->next;

	// Ενημέρωση του size και του first
	blist->size--;
	if (blist->first==node)
		blist->first = node->next;

	free(node);	
}

Pointer blist_find(BList blist, Pointer value, CompareFunc compare) {
	BListNode node = blist_find_node(blist, value, compare);
	return node == NULL ? NULL : node->value;
}

DestroyFunc blist_set_destroy_value(BList blist, DestroyFunc destroy_value) {
	DestroyFunc old = blist->destroy_value;
	blist->destroy_value = destroy_value;
	return old;
}

void blist_destroy(BList blist) {
	// Διάσχιση όλης της λίστας και free όλων των κόμβων
	// συμπεριλαμβανομένου του dummy
	BListNode node = blist->first;
	while (node!=NULL) {
		BListNode next = node->next;

		// Κλήση της destroy_value, αν υπάρχει (αλλά όχι στον dummy)
		if (node!=blist->dummy && blist->destroy_value!=NULL)
			blist->destroy_value(node->value);

		free(node);
		node = next;
	}

	// Τέλος, free το ίδιο το struct
	free(blist);
}


// Διάσχιση της λίστας /////////////////////////////////////////////

BListNode blist_first(BList blist) {
	// Αν η λίστα είναι κενή, τότε το first δείχνει τον dummy
	// Πρέπει όμως να επιστραφεί NULL, όχι ο dummy
	if (blist->first==blist->dummy)
		return BLIST_BOF;
	else
		return blist->first;
}

BListNode blist_last(BList blist) {
	// Ο τελευταίος κόμβος είναι ο προηγούμενος του dummy
	return blist->dummy->previous;
}

BListNode blist_next(BList blist, BListNode node) {
	assert(node!=NULL);		// LCOV_EXCL_LINE
	return node->next;
}

BListNode blist_previous(BList blist, BListNode node) {
	assert(node!=NULL);		// LCOV_EXCL_LINE
	return node->previous;
}

Pointer blist_node_value(BList blist, BListNode node) {
	assert(node!=NULL);		// LCOV_EXCL_LINE
	return node->value;
}

BListNode blist_find_node(BList blist, Pointer value, CompareFunc compare) {
	// Διάσχιση όλης της λίστας, καλώντας την compare μέχρι να επιστρέψει 0
	for (BListNode node=blist->first;
		node!=blist->dummy;
		node=node->next) {
			if (compare(value, node->value)==0) {
				return node;	// βρέθηκε
			}
		}
	return NULL;	// δεν υπάρχει
}
