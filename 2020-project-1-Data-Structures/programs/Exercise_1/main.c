#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Συνάρτηση που βρίσκει πόσες φορές
// υπάρχει η word στον πίνακα words
int search_word(char* word, char** words, int pos) {
    int sum = 0;

    for (int i=0 ; i<pos ; i++) {
        if (strcmp(word, words[i])==0) {
            sum++;
        }
    }

    return sum;
}

// Αποδεσμεύει την μνήνη του πίνακα words
void free_array(char** words, int size) {
    for (int i=0 ; i<size ; i++) {
        free(words[i]);
    }
    free(words);
}


int main(void) {
    char** words = malloc(100001*sizeof(char*));

    for (int i=0 ; i<100001 ; i++) {
        words[i] = malloc(20*sizeof(char));
    }

    // Διαβάζει γραμμές μέχρι το τέλος της εισόδου
    int i = 0;
    while (feof(stdin)==0) {
        fgets(words[i], 20, stdin);
        printf("%d\n", search_word(words[i], words, i));
        i++;
    }

    free_array(words, 100001);
    return 0;
}
