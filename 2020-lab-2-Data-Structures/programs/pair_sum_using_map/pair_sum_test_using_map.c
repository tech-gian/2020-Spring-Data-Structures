// pair_sum_test_using_map.c
//
// Test για το pair_sum.h module

#include "acutest.h"			// Απλή βιβλιοθήκη για unit testing

#include "pair_sum_using_map.h"


// Δεσμεύει μνήμη για έναν ακέραιο, αντιγράφει το value εκεί και επιστρέφει pointer
int* create_int(int value) {
	int* pointer = malloc(sizeof(int));		// δέσμευση μνήμης
	*pointer = value;						// αντιγραφή του value στον νέο ακέραιο
	return pointer;
}

void test_pair_sum_using_map() {
	int N = 100;

	Vector numbers = vector_create(0, free);
	for (int i = 0; i < N; i++)
		vector_insert_last(numbers, create_int(i));

	TEST_ASSERT(pair_sum_using_map(-1, numbers) == NULL);
	Pair pair = pair_sum_using_map(N, numbers);
	TEST_ASSERT(pair != NULL);
	TEST_ASSERT(pair->first + pair->second == N);

	free(pair);
	vector_destroy(numbers);
}


// Λίστα με όλα τα tests προς εκτέλεση
TEST_LIST = {
	{ "pair_sum_using_map", test_pair_sum_using_map },
	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
};