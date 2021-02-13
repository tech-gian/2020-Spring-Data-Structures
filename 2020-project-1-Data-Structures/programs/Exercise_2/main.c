#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ADTMap.h"

// Δέσμευση μνήμης για την word
char* create_word(int size) {
    char* word = malloc(size*sizeof(char));
    return word;
}

// Δέσμευση μνήμης για int
int* create_int(int value) {
    int* pointer = malloc(sizeof(int));
    *pointer = value;
    return pointer;
}

// CompareFunc για strings
int compare_strings(Pointer a, Pointer b) {
    return strcmp(a, b);
}


int main(void) {
    Map words = map_create(compare_strings, free, free);

    // Επανάληψη μέχρι το τέλος της εισόδου
    while(feof(stdin)==0) {
        char* word = create_word(200);
        fgets(word, 200, stdin);

        // insert στο map με αρχική τιμή
        if (map_find(words, word)==NULL) {
            map_insert(words, word, create_int(1));
            printf("0\n");
        }
        // ή με την προηγούμενη τιμή + 1
        else {
            int* value = map_find(words, word);
            printf("%d\n", *value);
            map_insert(words, word, create_int((*value)+1));
        }
    }

    map_destroy(words);
    return 0;
}