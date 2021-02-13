#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ADTSet.h"

// Δέσμευση μνήμης για string
char* create_string(int size) {
    char* str = malloc(200*sizeof(char));
    return str;
}

// CompareFunc για strings
int compare_strings(Pointer a, Pointer b) {
    return strcmp(a, b);
}

// Συνάρτηση που επιστρέφει την μικρότερη string,
// που είναι μεγαλύτερη από αυτήν που μόλις διαβάστηκε.
// Ή <none> σε διαφορετική περίπτωση
char* set_iteration(char* word, Set words) {

    for (SetNode node=set_first(words);
        node!=SET_EOF;
        node=set_next(words, node)) {
            char* temp = set_node_value(words, node);
            if (strcmp(word, temp)<=0) {
                return temp;
            }
        }
    return "<none>\n";
}


int main(void) {
    Set words = set_create(compare_strings, free);

    // Διαβάζω τις γραμές μέχρι 
    // το τέλος της εισόδου
    while (feof(stdin)==0) {
        char* word = create_string(200);
        fgets(word, 200, stdin);

        printf("%s", set_iteration(word, words));

        set_insert(words, word);
    }

    set_destroy(words);
    return 0;
}
