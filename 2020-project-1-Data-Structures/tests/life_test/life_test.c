#include <stdio.h>

#include "acutest.h"    // Απλή βιβλιοθήκη για unit testing

#include "life.h"


// Βοηθητική συνάρτηση

LifeState first_state(void) {
    // Δημιουργούμε manually το state
    // που υπάρχει στο test.rle
    LifeState state = life_create();
    
    LifeCell cell = {0, 1};
    life_set_cell(state, cell, true);
    
    cell.x = 1;
    cell.y = 2;
    life_set_cell(state, cell, true);

    cell.x = 2;
    for (int j=0 ; j<3 ; j++) {
        cell.y = j;
        life_set_cell(state, cell, true);
    }
    
    return state;
}


// Συναρτήσεις για test

void test_create(void) {
    // Δημιουργία κενού state
    LifeState state = life_create();

    // Έλεγχος ότι δεν επιστρέφει NULL
    TEST_CHECK(state!=NULL);

    life_destroy(state);
}

void test_set_and_get(void) {
    LifeState state = life_create();

    for (int i=0 ; i<10 ; i++) {
        for (int j=0 ; j<10 ; j++) {
            
            LifeCell cell = {i, j};

            // Με την life_set_cell αρχικοποιώ ένα τυχαίο state
            if (j%2==0) life_set_cell(state, cell, true);
            else life_set_cell(state, cell, false);

            // Με την life_get_cell ελέγχω το state αυτό
            if (j%2==0) TEST_CHECK(life_get_cell(state, cell)==true);
            else TEST_CHECK(life_get_cell(state, cell)==false);
        }
    }

    for (int i=10 ; i<20 ; i++) {
        for (int j=10 ; j<20 ; j++) {

            LifeCell cell = {i, j};

            // Αν δεν έχει οριστεί το cell,
            // τότε πρέπει να γυρνάει false
            TEST_CHECK(life_get_cell(state, cell)==false);
        }
    }

    life_destroy(state);
}


void test_create_from_rle(void) {
    LifeState state = first_state();

    // Δημιουργούμε το state από την συνάρτηση
    LifeState test_state = life_create_from_rle("test.rle");

    for (int i=0 ; i<3 ; i++) {
        for (int j=0 ; j<3 ; j++) {
            
            LifeCell test_cell = {i, j};
            bool value = life_get_cell(state , test_cell);
            bool test_value = life_get_cell(test_state, test_cell);

            // Ελέγχουμε αν έχει τις τιμές που πρέπει να έχει
            TEST_CHECK(value==test_value);
        }
    }
    
    life_destroy(state);
    life_destroy(test_state);
}


void test_save_to_rle(void) {
    LifeState state = first_state();

    // Το αποθηκεύουμε σε ένα .rle αρχείο
    life_save_to_rle(state, "test_save.rle");

    // Αφού έχουμε ελέγξει ότι δουλεύει σωστά,
    // δημιουργούμε test_state από το αρχείο που φτιάξαμε
    LifeState test_state = life_create_from_rle("test_save.rle");

    for (int i=0 ; i<3 ; i++) {
        for (int j=0 ; j<3 ; j++) {

            LifeCell test_cell = {i, j};
            bool value = life_get_cell(state, test_cell);
            bool test_value = life_get_cell(test_state, test_cell);

            // Ελέγχουμε αν έχει τις τιμές που πρέπει να έχει
            TEST_CHECK(value==test_value);
        }
    }

    life_destroy(state);
    life_destroy(test_state);
}


void test_evolve(void) {
    LifeState state = first_state();
    LifeCell cell;

    LifeState test_state = life_evolve(state);
    // Δημιουργούμε manually την επόμενη κατάσταση,
    // ώστε να ελέγξουμε ότι παράγει την σωστή
    cell.x = 0;
    cell.y = 1;
    life_set_cell(state, cell, false);

    cell.x = 1;
    cell.y = 0;
    life_set_cell(state, cell, true);

    cell.x = 2;
    life_set_cell(state, cell, false);

    cell.x = 3;
    cell.y = 1;
    life_set_cell(state, cell, true);

    for (int i=0 ; i<4 ; i++) {
        for (int j=0 ; j<4 ; j++) {

            LifeCell test_cell = {i, j};
            bool value = life_get_cell(state, test_cell);
            bool test_value = life_get_cell(test_state, test_cell);

            // Ελέγχουμε αν έχει τις τιμές που πρέπει να έχει
            TEST_CHECK(value==test_value);
        }
    }
    
    life_destroy(state);
    life_destroy(test_state);
}


// Tests για life_evolve_many (δεν τα ζητάει η εκφώνηση)


void test_evolve_many(void) {

    LifeState state = first_state();
    ListNode loop;

    // Χωρίς loop το pattern
    List list = life_evolve_many(state, 5, &loop);

    TEST_CHECK(loop==NULL);

    LifeCell cell;

    // Δημιουργούμε manually την επόμενη κατάσταση,
    // ώστε να ελέγξουμε ότι παράγει την σωστή
    state = life_create();

    cell.x = 2;
    cell.y = 1;
    life_set_cell(state, cell, true);

    cell.y = 3;
    life_set_cell(state, cell, true);

    cell.x = 3;
    cell.y = 2;
    life_set_cell(state, cell, true);

    cell.y = 3;
    life_set_cell(state, cell, true);

    cell.x = 4;
    cell.y = 2;
    life_set_cell(state, cell, true);

    LifeState test_state = list_node_value(list, list_last(list));

    for (int i=0 ; i<5 ; i++) {
        for (int j=0 ; j<5 ; j++) {

            LifeCell test_cell = {i, j};
            bool value = life_get_cell(state, test_cell);
            bool test_value = life_get_cell(test_state, test_cell);

            // Ελέγχουμε αν έχει τις τιμές που πρέπει να έχει
            TEST_CHECK(value==test_value);
        }
    }

    life_destroy(state);

    // Κάνουμε free τα LifeState που υπάρχουν στην λίστα
    for (ListNode node=list_first(list);
        node!=LIST_EOF;
        node=list_next(list, node)) {

            LifeState to_free = list_node_value(list, node);
            life_destroy(to_free);
        }

    list_destroy(list);
}


void test_evolve_many_loop(void) {

    // Ένα state που εμφανίζει loop
    LifeState state = life_create_from_rle("test_loop.rle");

    ListNode loop;
    List list = life_evolve_many(state, 5, &loop);

    // Ελέγχουμε το loop
    TEST_CHECK(loop!=NULL);
    TEST_CHECK(loop==list_first(list));

    LifeState test_state = list_node_value(list, loop);

    for (int i=0 ; i<9 ; i++) {
        for (int j=0 ; j<10 ; j++) {
            
            LifeCell test_cell = {i, j};
            bool value = life_get_cell(state, test_cell);
            bool test_value = life_get_cell(test_state, test_cell);

            // Ελέγχουμε αν έχει τις τιμές που πρέπει να έχει
            TEST_CHECK(value==test_value);
        }
    }

    // Κάνουμε free τα LifeState που υπάρχουν στην λίστα
    for (ListNode node=list_first(list);
        node!=LIST_EOF;
        node=list_next(list, node)) {

            LifeState to_free = list_node_value(list, node);
            life_destroy(to_free);
        }

    list_destroy(list);
}


// Λίστα με όλα τα tests προς εκτέλεση
TEST_LIST = {
    { "test_create", test_create }, 
    { "test_set_and_get", test_set_and_get },
    { "test_create_from_rle", test_create_from_rle },
    { "test_save_to_rle", test_save_to_rle },
    { "test_evolve", test_evolve },

    // Tests εξτρά, που δεν ζητούνταν στην εκφώνηση
    { "test_evolve_many", test_evolve_many },
    { "test_evolve_many_loop", test_evolve_many_loop },
    
    { NULL, NULL }  // τερματίζουμε με τη λίστα NULL
};
