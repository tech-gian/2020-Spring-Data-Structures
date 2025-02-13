// stats.h
// Απλά στατιστικά στοιχεία για πίνακες ακεραίων

#pragma once // #include το πολύ μία φοράν

#include <limits.h>     // σταθερές INT_MIN, INT_MAX

// Επιστρέφει το μικρότερο στοιχείο του array με μέγεθος size (ΙΝΤ_ΜΑΧ αν size == 0)

int stats_find_min(int array[], int size);

// Επιστρέφει το μεγαλύτερο στοιχείο του array με μέγεθος size (ΙΝΤ_MIN αν size == 0)

int stats_find_max(int array[], int size);

// Επιστρέφει το μεγαλύτερο από τα a, b, τα οποία είναι οποιουδήποτε τύπου
typedef void* Pointer;
typedef int (*CompareFunc)(Pointer a, Pointer b);


Pointer stats_generic_max(Pointer a, Pointer b, CompareFunc comp);
