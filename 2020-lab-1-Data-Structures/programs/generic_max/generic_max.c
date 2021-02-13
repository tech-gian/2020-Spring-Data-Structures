// Υλοποίηση μιας γενικής συνάρτησης max

#include <stdio.h>
#include "stats.h"
#include <string.h>


// Πιο κατανοητό να γράφουμε "Pointer" παρά "void*"
//typedef void* Pointer;

// Μια μεταβλητή τύπου CompareFunc είναι δείκτης σε συνάρτηση που
// συγκρίνει 2 στοιχεία a και b και επιστρέφει έναν ακέραιο με τιμή:
// < 0  αν a <  b
//   0  αν a == b
// > 0  αν a >  b


// Επιστρέφει το μεγαλύτερο από τα a, b, όπου η σύγκριση γίνεται
// με βάση τη συνάρτηση compare.


Pointer generic_max(Pointer a, Pointer b, CompareFunc comp) {
    // Καλούμε την comp για να μας συγκρίνει τους αριθμούς, επιστρέφουμε a ή b ανάλογα
    if(comp(a, b) > 0)
        return a;
    else
        return b;
}


// Συγκρίνει 2 strings a, b

int compare_strings(Pointer a, Pointer b) {
    return strcmp(a, b);
}

// Συγκρίνει 2 ακεραίους a, b

int compare_ints(Pointer a, Pointer b) {
    // αρνητικός, μηδέν ή θετικός, ανάλογα με τη διάταξη των a,b
    int* ia = a;
    int* ib = b;

    return *ia-*ib;
}

int main() {
    int a1 = 1;
    int a2 = 5;
    int* max_a = generic_max(&a1, &a2, compare_ints);

    printf("max of %d, %d is %d\n", a1, a2, *max_a);

    // Κάντε uncomment τα παρακάτω και συμπληρώστε κατάλληλα

    char *s1 = "zzz";
    char *s2 = "aaa";
    char *max_s = stats_generic_max(s1, s2, compare_strings);

    printf("max of %s , %s is %s\n", s1, s2, max_s);

    return 0;
}
// Καλώ στην μία περίπτωση την generic_max και στην άλλη την stats_generic_max
// Η μία είναι υλοποιημένη εδώ κι η άλλη στο stats.c