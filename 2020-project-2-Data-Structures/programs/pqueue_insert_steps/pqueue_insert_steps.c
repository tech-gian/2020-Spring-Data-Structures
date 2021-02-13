#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ADTPriorityQueue.h"

// Βοηθητικές Συναρτήσεις

int compate_ints(Pointer a, Pointer b) {
    return *(int*)a - *(int*)b;
}

int* create_int(int value) {
    int* pointer = malloc(sizeof(int));
    *pointer = value;
    return pointer;
}

// pqueue_insert_steps

int main(int argc, char* argv[]) {
    // Δημιουργούμε το αρχείο .csv
    FILE* f = fopen("pqueue_graph.csv", "w");

    PriorityQueue pqueue = pqueue_create(compate_ints, free, NULL);

    // Αν ψάχνουμε για real-time complexity
    if (strcmp(argv[1], "real")==0) {
        for (int i=1 ; i<10000 ; i++) {
            pqueue_insert(pqueue, create_int(i));

            fprintf(f, "%d,%d\n", i, pqueue_steps(pqueue));
        }
    }
    // Αν ψάχνουμε για amortized-time complexity
    else if (strcmp(argv[1], "amortized")==0) {
        int sum = 0;
        for (int i=1 ; i<10000 ; i++) {
            pqueue_insert(pqueue, create_int(i));
            sum += pqueue_steps(pqueue);

            float average = ((float) sum)/((float) i);
            fprintf(f, "%d,%f\n", i, average);
        }
    }

    pqueue_destroy(pqueue);
    fclose(f);
    return 0;
}