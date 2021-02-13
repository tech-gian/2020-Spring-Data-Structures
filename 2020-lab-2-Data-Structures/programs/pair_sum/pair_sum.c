#include <stdlib.h>

#include "pair_sum.h"


Pair pair_sum(int target, Vector numbers) {
	// Προς υλοποίηση
	Pair pair = malloc(sizeof(struct pair));
//αναζήτηση του a και του b στον vector
//ώστε να έχουν άθροισμα ίσο με target
	for (int i=0 ; i<vector_size(numbers) ; i++) {
		for (int j=i ; j<vector_size(numbers); j++) {
			
			int* a = vector_get_at(numbers, i);
			int* b = vector_get_at(numbers, j);

			int sum = *(a) + *(b);
			if (target==sum) {
				pair->first = *(a);
				pair->second = *(b);
				return pair;
			}
		}
	}
//add tests if you want
	return NULL;
}