#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ADTVector.h"

// Βοηθητική συνάρτηση

int* create_int(int value) {
    int* pointer = malloc(sizeof(int));
    *pointer = value;
    return pointer;
}

// vector_insert_steps

int main(int argc, char* argv[]) {
    // Δημιουργούμε το αρχείο .csv
    FILE* f = fopen("vector_graph.csv", "w");

    Vector vec = vector_create(0, free);

    // Αν ψάχνουμε την real-time complexity
    if (strcmp(argv[1], "real")==0) {
        for (int i=1 ; i<=10000 ; i++) {
            vector_insert_last(vec, create_int(i));

            fprintf(f, "%d,%d\n", i, vector_steps(vec));
        }
    }
    // Αν ψάχνουμε την amortized-time complexity
    else if (strcmp(argv[1], "amortized")==0) {
        int sum = 0;
        for (int i=1 ; i<=10000 ; i++) {
            vector_insert_last(vec, create_int(i));
            sum += vector_steps(vec);

            float average = ((float) sum)/((float) i);
            fprintf(f, "%d,%f\n", i, average);
        }
    }

    vector_destroy(vec);
    fclose(f);
    return 0;
}
