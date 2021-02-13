#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "life.h"


// common functions

// Συγκρίνει 2 LifeStates
int compare_states(Pointer a, Pointer b) {
    LifeState state_a = (LifeState) a;
    LifeState state_b = (LifeState) b;

    for (MapNode node=map_first(state_a);
        node!=MAP_EOF;
        node=map_next(state_a, node)) {

            LifeCell* cell = map_node_key(state_a, node);
            bool vala = life_get_cell(state_a, *cell);
            bool valb = life_get_cell(state_b, *cell);

            if (vala!=valb) return vala-valb;
        }
    
    return 0;
}

// Δεσμεύει χώρο στη μνήμη για int
int* create_int(int value) {
    int* pointer = malloc(sizeof(int));
    *pointer = value;
    return pointer;
}

// Δεσμεύει χώρο στη μνήμη για bool
bool* create_bool(bool value) {
    bool* pointer = malloc(sizeof(bool));
    *pointer = value;
    return pointer;
}

// Συγκρίνει 2 LifeCells
int compare_cells(Pointer a, Pointer b) {
    LifeCell cell_a = *(LifeCell*) a;
    LifeCell cell_b = *(LifeCell*) b;

    if (cell_a.x==cell_b.x) {
        return cell_a.y - cell_b.y;
    }

    return cell_a.x - cell_b.x;
}

// Δεσμεύει χώρο στη μνήμη για cell
LifeCell* create_cell(LifeCell cell) {
    LifeCell* pointer = malloc(sizeof(LifeCell));
    *pointer = cell;
    return pointer; 
}


// real functions for life


LifeState life_create() {
    // Ένα LifeState είναι απλά ένα Map, με αντιστοιχία
    // LifeCell (key) -> bool (value)
    Map basic =  map_create(compare_cells, free, free);
    return basic;
}


LifeState life_create_from_rle(char* file) {
    FILE* f;
    LifeState state = life_create();

    f = fopen(file, "r");

    int i = 0;
    int j = 0;
    char c = fgetc(f);

    // Αν η γραμμή αρχίζει με #
    // ή με x = , y =
    while (c=='#') {
        char buf[1024];
        fgets(buf, 1024, f);
        c = fgetc(f);
    }
    if (c=='x') {
        char buf[1024];
        fgets(buf, 1024, f);
        c = fgetc(f);
    }

    // Παίρνουμε τις τιμές από το .rle αρχείο 
    // Με το c, ελέγχουμε τι χαρακτήρας είναι
    while (c!='!') {
        // Αν είναι νεκρό
        if (c=='b') {
            j++;
        }
        // Αν είναι ζωντανό
        else if (c=='o') {
            LifeCell cell = {i, j};
            life_set_cell(state, cell, true);
            j++;
        }
        // Αν αλλάζει γραμμή
        else if (c=='$') {
            i++;
            j = 0;
        }
        // Αν είναι αριθμός
        else if (c>='0' && c<='9') {
            int num = c - '0';

            c = fgetc(f);
            while (c>='0' && c<='9') {
                num *= 10;
                num += c - '0';
                c = fgetc(f);
            }

            // Ελέγχουμε τι υπάρχει μετά τον αριθμό
            bool value;
            if (c=='b') value = false;
            else if (c=='o') value = true;
            else if (c=='$') {
                i += num;
                j = 0;
                num = 0;
            }

            while (num--) {
                // Το περνάμε στο LifeState μόνο αν είναι ζωντανό
                if (value==1 && c!='$') {
                    LifeCell cell = {i, j};
                    life_set_cell(state, cell, value);
                }
                j++;
            }
        }
        c = fgetc(f);
    }

    fclose(f);
    return state;
}

void life_save_to_rle(LifeState state, char* file) {
    FILE* f = fopen(file, "w");

    int i_min = INT_MAX;
    int i_max = INT_MIN;
    int j_min = INT_MAX;
    int j_max = INT_MIN;

    // Βρίσκουμε τα όρια του LifeState
    // από τα όρια των ζωντανών LifeCells
    for (MapNode node=map_first(state);
        node!=MAP_EOF;
        node=map_next(state, node)) {

        LifeCell* cell = map_node_key(state, node);
        bool* value = map_node_value(state, node);

        if (*value==1) {
            if (cell->x<i_min) i_min = cell->x;
            if (cell->x>i_max) i_max = cell->x;
            if (cell->y<j_min) j_min = cell->y;
            if (cell->y>j_max) j_max = cell->y;
        }
    }


    int lines = 0;

    // Διατρέχουμε τα όρια αυτά
    // και αντίστοιχα φτιάχνουμε το αρχείο .rle
    for (int i=i_min ; i<=i_max ; i++) {
        
        // counter = πόσα συνεχόμενα ίδια κελιά
        int counter = 0;
        // Το flag για να ελέγχουμε αν μια
        // σειρά έχει μόνο νεκρά κελιά
        int flag = 0;
        for (int j=j_min ; j<=j_max ; j++) {
            LifeCell cell = {i, j};

            // Παίρνουμε το value του κελιού
            bool value = life_get_cell(state, cell);
            cell.y++;
            // και του επόμενου του
            bool next_value = life_get_cell(state, cell);

            // Εκτύπωση αλλαγή γραμμής ή γραμμών
            if ((value==true || next_value==true) && flag==0) {
                flag = 1;
                if (lines!=0) {
                    fprintf(f, "%d$", lines+1);
                    lines = 0;
                }
                else if (i!=i_min) fprintf(f, "$");
            }

            // Εκτύπωση ζωντανού ή νεκρού κελιού
            // (αντίστοιχα κελιών, αν είναι συνεχόμενα)
            if (value!=next_value) {
                if (counter!=0) fprintf(f, "%d", counter+1);

                if (value==true) fprintf(f, "o");
                if (value==false) fprintf(f, "b");
                
                counter = 0;
            }
            else {
                counter++;
            }

        }
        if (i==i_max) break;

        if (flag==0) lines++;
    }

    fprintf(f, "!");

    fclose(f);
}


bool life_get_cell(LifeState state, LifeCell cell) {
    // Παίρνουμε την τιμή για την θέση (cell) που μας δίνεται
    bool* value = map_find(state, &cell);
    if (value==NULL) {
        return false;
    }
    return *value; // δηλαδή true
}


void life_set_cell(LifeState state, LifeCell cell, bool value) {
    // Ορίζουμε την τιμή που μας δίνεται στη θέση (cell)
    map_insert(state, create_cell(cell), create_bool(value));
}


LifeState life_evolve(LifeState state) {
    LifeState next_state = life_create();

    int i_min = INT_MAX;
    int i_max = INT_MIN;
    int j_min = INT_MAX;
    int j_max = INT_MIN;

    // Βρίσκουμε τα όρια του LifeState
    // από τα όρια των ζωντανών LifeCells
    for (MapNode node=map_first(state);
        node!=MAP_EOF;
        node=map_next(state, node)) {

        LifeCell* cell = map_node_key(state, node);
        bool* value = map_node_value(state, node);

        if (*value==1) {
            if (cell->x<i_min) i_min = cell->x;
            if (cell->x>i_max) i_max = cell->x;
            if (cell->y<j_min) j_min = cell->y;
            if (cell->y>j_max) j_max = cell->y;
        }
    }

    // Από τα όρια αυτά (με μία γραμμή εξτρά πάνω, κάτω, αριστερά, δεξιά)
    // ελέγχουμε ποια κελία πρέπει να αλλάξουμε 
    for (int i=i_min-1 ; i<=i_max+1 ; i++) {
        for (int j=j_min-1 ; j<=j_max+1 ; j++) {
            LifeCell cell = {i, j};
            bool value = life_get_cell(state, cell);

            int counter = 0;
            LifeCell temp;  

        // Ακολουθούμε τους κανόνες για τον έλεγχο κάθε cell
            temp.x = i-1;
            temp.y = j-1;
            if (life_get_cell(state, temp)==1) counter++;

            temp.x = i-1;
            temp.y = j;
            if (life_get_cell(state, temp)==1) counter++;

            temp.x = i-1;
            temp.y = j+1;
            if (life_get_cell(state, temp)==1) counter++;

            temp.x = i;
            temp.y = j-1;
            if (life_get_cell(state, temp)==1) counter++;

            temp.x = i;
            temp.y = j+1;
            if (life_get_cell(state, temp)==1) counter++;

            temp.x = i+1;
            temp.y = j-1;
            if (life_get_cell(state, temp)==1) counter++;

            temp.x = i+1;
            temp.y = j;
            if (life_get_cell(state, temp)==1) counter++;

            temp.x = i+1;
            temp.y = j+1;
            if (life_get_cell(state, temp)==1) counter++;

            // Βάζουμε τιμή μόνο στα ζωντανά που μένουν
            if (value==1 && (counter>=2 && counter<=3)) {
                life_set_cell(next_state, cell, true);
            }
            else if (value==0 && counter==3) {
                life_set_cell(next_state, cell, true);
            }
        }
    }

    return next_state;

}


void life_destroy(LifeState state) {
    // Αποδεσμεύει την μνήμη του LifeState
    map_destroy(state);
}


// Exercise_6


List life_evolve_many(LifeState state, int steps, ListNode* loop) {
    // Εισαγωγή στη λίστα της πρώτης κατάστασης
    List list = list_create(NULL);
    list_insert_next(list, list_last(list), state);
    *loop = NULL;

    // Εισαγωγή στο map της πρώτης κατάστασης
    // ώστε να έχω γρήγορη αναζήτηση
    Map finder = map_create(compare_states, NULL, free);
    map_insert(finder, state, create_int(steps));


    while (steps--) {
        state = life_evolve(state);

        // Επανάληψη ώστε να παραχθούν όλες οι καταστάσεις
        if (map_find(finder, state)==NULL) {
            map_insert(finder, state, create_int(steps));
            list_insert_next(list, list_last(list), state);
        }
        // ή αν υπάρχει loop, το αποθηκεύω
        else {
            *loop = list_first(list);
            // Αν βρει loop, κάνω free την τελευταία
            // κατάσταση, αφού δεν χρειάζεται πουθενά
            life_destroy(state);
            break;
        }
    }

    map_destroy(finder);
    return list;
}
