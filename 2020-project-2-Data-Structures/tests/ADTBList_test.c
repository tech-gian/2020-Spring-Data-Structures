//////////////////////////////////////////////////////////////////
//
// Unit tests για τον ADT BList.
// Οποιαδήποτε υλοποίηση οφείλει να περνάει όλα τα tests.
//
//////////////////////////////////////////////////////////////////

#include "acutest.h"			// Απλή βιβλιοθήκη για unit testing

#include "ADTBList.h"


void test_create(void) {
	// Δημιουργία κενής λίστας
	BList blist = blist_create(NULL);
	blist_set_destroy_value(blist, NULL);

	// Έλεγχος ότι η malloc δεν απέτυχε
	// και ότι το μέγεθος της λίστας είναι 0
	TEST_CHECK(blist!=NULL);
	TEST_CHECK(blist_size(blist)==0);

	blist_destroy(blist);
}

void test_insert(void) {
	BList blist = blist_create(NULL);

	int N = 1000;
	int array[N];

	for (int i=0 ; i<N ; i ++) {
		// Προσθέτω τους δείκτες των στοιχείων του πίνακα
		// στο τέλος της λίστας
		blist_insert(blist, BLIST_EOF, &array[i]);

		// Έλεγχος αν αυξήθηκε το μέγεθος
		TEST_CHECK(blist_size(blist) == (i+1));

		// Έλεγχος ότι ο τελευταίος κόμβος έχει την σωστή τιμή
		TEST_CHECK(blist_node_value(blist, blist_last(blist)) == &array[i]);
	}

	// Έλεγχος αν τα στοιχεία έχουν μπει με την σειρά που πρέπει
	BListNode node = blist_last(blist);

	// Για να ελέγξω και το previous
	for (int i=N-1 ; i>=0 ; i--) {
		TEST_CHECK(blist_node_value(blist, node) == &array[i]);
		node = blist_previous(blist, node);
	}

	// Εισαγωγή σε ενδιάμεσο κόμβο
	blist_insert(blist, blist_next(blist, blist_first(blist)), NULL);
	TEST_CHECK(blist_node_value(blist, blist_next(blist, blist_first(blist))) == NULL);

	blist_destroy(blist);
}

void test_remove(void) {
	// Δημιουργία λίστας με free σαν destroy_value
	BList blist = blist_create(free);

	int N = 1000;
	int* array[N];

	// Βάζω στοιχεία στην λίστα με blist_insert
	for (int i=0 ; i<N ; i++) {
		// Δυναμική δέσμευση για να ελεγχθεί η destroy_function
		array[i] = malloc(sizeof(int));

		*array[i] = i;
		blist_insert(blist, blist_first(blist), array[i]);
	}

	// Διαγραφή από την αρχή και έλεγχος αν ο πρώτος κόμβος
	// έχει κάθε φορά την σωστή τιμή
	for (int i=N-1 ; i>=0 ; i--) {
		TEST_CHECK(blist_node_value(blist, blist_first(blist)) == array[i]);
		blist_remove(blist, blist_first(blist));

		// Έλεγχος ενημέρωσης του μεγέθους
		TEST_CHECK(blist_size(blist) == i);
	}

	// Ξαναγέμισμα της λίστας για remove του τελευταίου στοιχείου
	// και ενός ενδιάμεσου
	for (int i=0 ; i<N ; i++) {
		array[i] = malloc(sizeof(int));
		*array[i] = i;
		blist_insert(blist, blist_first(blist), array[i]);
	}

	// remove του τελευταίου στοιχείου
	blist_remove(blist, blist_last(blist));
	TEST_CHECK(blist_size(blist) == N-1);

	// remove του δεύτερου στοιχείου
	blist_remove(blist, blist_next(blist, blist_first(blist)));
	TEST_CHECK(blist_size(blist) == N-2);

	// remove του προτελευταίου στοιχείου
	blist_remove(blist, blist_previous(blist, blist_last(blist)));
	TEST_CHECK(blist_size(blist) == N-3);

	blist_destroy(blist);
}


// Βοηθητική συνάρτηση

int compare_ints(Pointer a, Pointer b) {
	return *(int*)a - *(int*)b;
}

void test_find() {
	BList blist = blist_create(NULL);
	int N = 1000;
	int array[N];

	// Εισαγωγή τιμών στον πίνακα για έλεγχο της blist_find
	for (int i=0 ; i<N ; i++) {
		array[i] = i;
		blist_insert(blist, blist_first(blist), &array[i]);
	}

	// Εύρεση όλων των στοιχείων
	for (int i=0 ; i<N ; i++) {
		int* value = blist_find(blist, &i, compare_ints);
		TEST_CHECK(value == &array[i]);
	}

	// Έλεγχος για μια τιμή που δεν υπάρχει στην λίστα
	// Πρέπει να επιστρέφει NULL
	int not_exists = -1;
	TEST_CHECK(blist_find(blist, &not_exists, compare_ints) == NULL);

	blist_destroy(blist);
}

void test_find_node() {
	BList blist = blist_create(NULL);

	int N = 1000;
	int array[N];

	// Εισαγωγή τιμών στον πίνακα και στην λίστα (στην αρχή)
	for (int i=0 ; i<N ; i++) {
		array[i] = i;
		blist_insert(blist, blist_first(blist), &array[i]);
	}


	BListNode node = blist_first(blist);

	// Έλεγχος ότι η blist_find_node βρίσκει σωστά τον πρώτο κόμβο
	// με value τον δείκτη &array[i]
	for (int i=N-1 ; i>=0 ; i--) {
		BListNode found = blist_find_node(blist, &i, compare_ints);
		TEST_CHECK(found == node);
		TEST_CHECK(blist_node_value(blist, found) == &array[i]);

		// Προσπέλαση όλης της λίστας για έλεγχο όλων των δεικτών του array
		node = blist_next(blist, node);
	}

	blist_destroy(blist);
}


// Λίστα με όλα τα tests προς εκτέλεση
TEST_LIST = {
	{ "blist_create", test_create },
	{ "blist_insert", test_insert },
	{ "blist_remove", test_remove },
	{ "blist_find", test_find },
	{ "blist_find_node", test_find_node },
	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
};
