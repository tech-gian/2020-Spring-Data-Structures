#include <stdio.h>
#include <stdlib.h>

#include "pair_sum_using_map.h"
#include "ADTMap.h"
#include "ADTVector.h"


int compare_ints(Pointer a, Pointer b) {
    return *(int*)a - *(int*)b;
}

Pair pair_sum_using_map(int target, Vector numbers) {
//δημιουργία map
    Map map = map_create(compare_ints, free, free);

    for (int i=0 ; i<vector_size(numbers) ; i++) {
        int* value = vector_get_at(numbers, i);
        map_insert(map, value, value);
    }
//ψάχνω το b στο map μέσω της map_find
//και επιστρέφω το pair αν το βρω ή NULL
//σε αντίθετη περίπτωση
    for (int i=0 ; i<vector_size(numbers) ; i++) {
        int* a= vector_get_at(numbers, i);
        int b = target - *(a);
        
        if (map_find(map, &b)!=NULL) {
            Pair pair = malloc(sizeof(struct pair));
            pair->first = *(a);
            pair->second = b;
            return pair;
        }        
    }
    return NULL;
}