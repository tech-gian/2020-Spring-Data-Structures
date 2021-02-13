//////////////////////////////////////////////////////////////////
//
// Unit tests για τον ADT Graph.
// Οποιαδήποτε υλοποίηση οφείλει να περνάει όλα τα tests.
//
//////////////////////////////////////////////////////////////////

#include "acutest.h"			// Απλή βιβλιοθήκη για unit testing

#include "ADTGraph.h"


// Ειδική CompareFunc για int
int compare_ints(Pointer a, Pointer b) {
	return *(int*)a - *(int*)b;
}

// Δέσμευση μνήμης για int
int* create_int(int value) {
	int* pointer = malloc(sizeof(int));
	*pointer = value;
	return pointer;
}


// MAYBE it needs something more
void test_create(void) {
	// Δημιουργούμε ένα κενό Graph (χωρίς DestroyFunc)
	Graph graph = graph_create(compare_ints, NULL);
	graph_set_hash_function(graph, hash_int);

	// Ελέγχουμε ότι δεν απέτυχε η malloc στην λίστα, και ότι
	// αρχικοποιείται με μέγεθος 0 (δηλαδή χωρίς κόμβους)
	TEST_ASSERT(graph != NULL);
	TEST_ASSERT(graph_size(graph) == 0);

	graph_destroy(graph);
}



void test_insert_vertices(void) {
	// Δημιουργούμε ένα κενό Graph
	Graph graph = graph_create(compare_ints, free);
	graph_set_hash_function(graph, hash_int);

	int N = 1000;
	int** array = malloc(N*sizeof(int*));

	// Κάνουμε insert 1000 κορυφές
	for (int i=0 ; i<N ; i++) {
		array[i] = create_int(i);
		graph_insert_vertex(graph, array[i]);
	}
	
	TEST_ASSERT(graph_size(graph) == N);

	// Ελέγχουμε κάθε κόμβο της λίστας
	// ότι περιέχει τον σωστό κόμβο
	List vertices = graph_get_vertices(graph);
	TEST_ASSERT(list_size(vertices) == N);

	int i = 0;
	for (ListNode node=list_first(vertices);
		node!=LIST_EOF;
		node=list_next(vertices, node)) {
			TEST_ASSERT(list_node_value(vertices, node) == array[i]);
			i++;
		}
	
	list_destroy(vertices);

	free(array);
	graph_destroy(graph);
}


// Βοηθητική συνάρτηση δημιουργίας ενός γράφου
static Graph graph_with_edges(int** array, int n) {
	Graph graph = graph_create(compare_ints, free);
	graph_set_hash_function(graph, hash_int);

	// Αρχικοποιούμε τον γράφο με n κορυφές
	for (int i=0 ; i<n ; i++) {
		array[i] = create_int(i);
		graph_insert_vertex(graph, array[i]);
	}

	// Κάνουμε insert τις ακμές του γράφου
	graph_insert_edge(graph, array[0], array[1], 0);
	graph_insert_edge(graph, array[0], array[2], 0);
	graph_insert_edge(graph, array[0], array[4], 0);
	graph_insert_edge(graph, array[1], array[3], 0);
	graph_insert_edge(graph, array[1], array[2], 0);
	graph_insert_edge(graph, array[2], array[4], 0);

	return graph;
}




void test_insert_edges(void) {
	int** array = malloc(5*sizeof(int*));

	// Ο γράφος δεν έχει βάρη. Αυτό θα ελεγχθεί στη graph_shortest_path
	Graph graph = graph_with_edges(array, 5);
	TEST_ASSERT(graph_size(graph) == 5);
	

	// Γίνεται έλεγχος για κάθε κορυφή ότι
	// έχει συνδεθεί με τις σωστές κορυφές(γείτονες).
	// Δηλαδή ότι υπάρχουν οι σωστές ακμές στον γράφο


	List adj_list = graph_get_adjacent(graph, array[0]);
	TEST_ASSERT(list_find(adj_list, array[1], compare_ints) != NULL);
	TEST_ASSERT(list_find(adj_list, array[2], compare_ints) != NULL);
	TEST_ASSERT(list_find(adj_list, array[4], compare_ints) != NULL);
	TEST_ASSERT(list_size(adj_list) == 3);
	list_destroy(adj_list);

	adj_list = graph_get_adjacent(graph, array[1]);
	TEST_ASSERT(list_find(adj_list, array[0], compare_ints) != NULL);
	TEST_ASSERT(list_find(adj_list, array[2], compare_ints) != NULL);
	TEST_ASSERT(list_find(adj_list, array[3], compare_ints) != NULL);
	TEST_ASSERT(list_size(adj_list) == 3);
	list_destroy(adj_list);

	adj_list = graph_get_adjacent(graph, array[2]);
	TEST_ASSERT(list_find(adj_list, array[0], compare_ints) != NULL);
	TEST_ASSERT(list_find(adj_list, array[1], compare_ints) != NULL);
	TEST_ASSERT(list_find(adj_list, array[4], compare_ints) != NULL);
	TEST_ASSERT(list_size(adj_list) == 3);
	list_destroy(adj_list);

	adj_list = graph_get_adjacent(graph, array[3]);
	TEST_ASSERT(list_find(adj_list, array[1], compare_ints) != NULL);
	TEST_ASSERT(list_size(adj_list) == 1);
	list_destroy(adj_list);

	adj_list = graph_get_adjacent(graph, array[4]);
	TEST_ASSERT(list_find(adj_list, array[0], compare_ints) != NULL);
	TEST_ASSERT(list_find(adj_list, array[2], compare_ints) != NULL);
	TEST_ASSERT(list_size(adj_list) == 2);
	list_destroy(adj_list);

	free(array);
	graph_destroy(graph);
}


void test_remove_vertices(void) {
	// Δημιουργούμε έναν κενό Graph
	Graph graph = graph_create(compare_ints, free);
	graph_set_hash_function(graph, hash_int);

	int N = 1000;
	int** array = malloc(N*sizeof(int*));

	// Τον αρχικοποιούμε με N κορυφές
	// (χωρίς καμία ακμή)
	for (int i=0 ; i<N ; i++) {
		array[i] = create_int(i);
		graph_insert_vertex(graph, array[i]);
	}

	// Αφαιρούμε τις κορυφές μία-μία
	// και ελέγχουμε ότι μειώνεται το size
	for (int i=N-1 ; i>=0 ; i--) {
		graph_remove_vertex(graph, array[i]);
		TEST_ASSERT(graph_size(graph) == i);
		// MAYBE add vertices_list
	}

	free(array);
	graph_destroy(graph);
}


void test_remove_edges(void) {
	int** array = malloc(5*sizeof(int*));

	// Ο γράφος δεν έχει βάρη. Αυτό θα ελεγχθεί στη graph_shortest_path
	Graph graph = graph_with_edges(array, 5);
	TEST_ASSERT(graph_size(graph) == 5);


	// Ο γράφος περιέχει ακμές, οι οποίες συνδέουν τις κορυφές.
	// Κάνοντας remove_vertex, πρέπει να αφαιρεθούν και
	// οι αντίστοιχες ακμές(συνδέσεις με τους γείτονες).

	graph_remove_vertex(graph, array[1]);

	List adj_list = graph_get_adjacent(graph, array[0]);
	TEST_ASSERT(list_size(adj_list) == 2);
	list_destroy(adj_list);

	adj_list = graph_get_adjacent(graph, array[2]);
	TEST_ASSERT(list_size(adj_list) == 2);
	list_destroy(adj_list);

	adj_list = graph_get_adjacent(graph, array[3]);
	TEST_ASSERT(list_size(adj_list) == 0);
	list_destroy(adj_list);

	graph_destroy(graph);


	// Δεύτερος γράφος (για μεγαλύτερη ακρίβεια του test)
	// Επαναλαμβάνεται η ίδια διαδικασία, απλά τώρα
	// αφαιρούνται ακμές και όχι κορυφές.
	// Ελέγχεται η ορθότητα της remove_edge

	graph = graph_with_edges(array, 5);
	TEST_ASSERT(graph_size(graph) == 5);

	// Πρώτη αφαίρεση ακμής
	graph_remove_edge(graph, array[0], array[2]);

	adj_list = graph_get_adjacent(graph, array[0]);
	TEST_ASSERT(list_size(adj_list) == 2);
	list_destroy(adj_list);

	adj_list = graph_get_adjacent(graph, array[2]);
	TEST_ASSERT(list_size(adj_list) == 2);
	list_destroy(adj_list);

	// Δεύτερη αφαίρεση ακμής
	graph_remove_edge(graph, array[0], array[4]);

	adj_list = graph_get_adjacent(graph, array[0]);
	TEST_ASSERT(list_size(adj_list) == 1);
	list_destroy(adj_list);

	adj_list = graph_get_adjacent(graph, array[4]);
	TEST_ASSERT(list_size(adj_list) == 1);
	list_destroy(adj_list);

	// Αποδέσμευση μνήμης
	free(array);
	graph_destroy(graph);
}


Graph graph_with_weights(int** array, int n) {
	Graph graph = graph_create(compare_ints, free);
	graph_set_hash_function(graph, hash_int);

	// Αρχικοποίση του γράφου με n κορυφές
	for (int i=0 ; i<n ; i++) {
		array[i] = create_int(i);
		graph_insert_vertex(graph, array[i]);
	}

	// Σύνδεση των κορυφών, μέσω ακμών.
	// Προστίθενται και κάποια βάρη(weights)
	graph_insert_edge(graph, array[0], array[1], 4);
	graph_insert_edge(graph, array[0], array[2], 30);
	graph_insert_edge(graph, array[0], array[4], 5);
	graph_insert_edge(graph, array[1], array[3], 20);
	graph_insert_edge(graph, array[1], array[2], 3);
	graph_insert_edge(graph, array[2], array[4], 90);

	return graph;
}


void test_shortest_path(void) {
	int** array = malloc(5*sizeof(int*));

	// Δημιουργία του γράφου
	Graph graph = graph_with_weights(array, 5);
	TEST_ASSERT(graph_size(graph) == 5);

	// Έλεγχος για σωστή εισαγωγή των weights
	TEST_ASSERT(graph_get_weight(graph, array[0], array[1]) == 4);
	TEST_ASSERT(graph_get_weight(graph, array[1], array[0]) == 4);
	TEST_ASSERT(graph_get_weight(graph, array[0], array[2]) == 30);
	TEST_ASSERT(graph_get_weight(graph, array[2], array[0]) == 30);
	TEST_ASSERT(graph_get_weight(graph, array[0], array[4]) == 5);
	TEST_ASSERT(graph_get_weight(graph, array[4], array[0]) == 5);
	TEST_ASSERT(graph_get_weight(graph, array[1], array[3]) == 20);
	TEST_ASSERT(graph_get_weight(graph, array[3], array[1]) == 20);
	TEST_ASSERT(graph_get_weight(graph, array[1], array[2]) == 3);
	TEST_ASSERT(graph_get_weight(graph, array[2], array[1]) == 3);
	TEST_ASSERT(graph_get_weight(graph, array[2], array[4]) == 90);
	TEST_ASSERT(graph_get_weight(graph, array[4], array[2]) == 90);
	

	// Έλεγχος (του ίδου γράφου) για το βέλτιστο (γρηγορότερο) μονοπάτι

	// 1ο ζευγάρι κορυφών που ελέγχεται
	List path = graph_shortest_path(graph, array[4], array[2]);
	TEST_ASSERT(list_size(path) == 4);

	TEST_ASSERT(*(int*)list_node_value(path, list_first(path)) == *(int*)array[4]);
	TEST_ASSERT(*(int*)list_node_value(path, list_next(path, list_first(path))) == *(int*)array[0]);
	TEST_ASSERT(*(int*)list_node_value(path, list_next(path, list_next(path, list_first(path)))) == *(int*)array[1]);
	TEST_ASSERT(*(int*)list_node_value(path, list_last(path)) == *(int*)array[2]);

	list_destroy(path);

	// 2ο ζευγάρι κορυφών που ελέγχεται
	path = graph_shortest_path(graph, array[1], array[3]);
	TEST_ASSERT(list_size(path) == 2);

	TEST_ASSERT(*(int*)list_node_value(path, list_first(path)) == *(int*)array[1]);
	TEST_ASSERT(*(int*)list_node_value(path, list_last(path)) == *(int*)array[3]);

	list_destroy(path);

	// 3ο ζευγάρι κορυφών που ελέγχεται
	path = graph_shortest_path(graph, array[2], array[0]);
	TEST_ASSERT(list_size(path) == 3);

	TEST_ASSERT(*(int*)list_node_value(path, list_first(path)) == *(int*)array[2]);
	TEST_ASSERT(*(int*)list_node_value(path, list_next(path, list_first(path))) == *(int*)array[1]);
	TEST_ASSERT(*(int*)list_node_value(path, list_last(path)) == *(int*)array[0]);

	list_destroy(path);

	free(array);
	graph_destroy(graph);


	// Δημιουργία γράφου χωρίς ακμές
	graph = graph_create(compare_ints, free);
	graph_set_hash_function(graph, hash_int);

	array = malloc(10*sizeof(int*));
	for (int i=0 ; i<10 ; i++) {
		array[i] = create_int(i);
		graph_insert_vertex(graph, array[i]);
	}

	// Η shortest_path πρέπει να επιστρέφει κενή λίστα
	// σε κάθε περίπτωση, αφού καμία κορυφή δεν
	// συνδέεται με καμία κορυφή

	// 1ο ζευγάρι κορυφών που ελέγχεται
	path = graph_shortest_path(graph, array[0], array[1]);
	TEST_ASSERT(list_size(path) == 0);

	list_destroy(path);

	// // 2ο ζευγάρι κορυφών που ελέγχεται
	path = graph_shortest_path(graph, array[4], array[2]);
	TEST_ASSERT(list_size(path) == 0);

	list_destroy(path);
	
	// Αποδέσμευση μνήμης 
	free(array);
	graph_destroy(graph);
}



// Λίστα με όλα τα tests προς εκτέλεση
TEST_LIST = {
	{ "graph_create", test_create },
	{ "graph_insert_vertices", test_insert_vertices },
	{ "graph_insert_edges", test_insert_edges },
	{ "graph_remove_vertices", test_remove_vertices },
	{ "graph_remove_edges", test_remove_edges },
	{ "graph_shortest_path", test_shortest_path },

	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
};
