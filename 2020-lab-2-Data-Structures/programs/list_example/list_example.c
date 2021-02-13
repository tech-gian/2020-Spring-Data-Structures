#include <stdlib.h>
#include <stdio.h>

#include "ADTList.h"

int* create_int(int value) {
    int* pointer = malloc(sizeof(int)); // δέσμευση μνήμης
    *pointer = value;                   // αντιγραφή του value στον νέο ακέραιο
    return pointer;
}

int main() {
	List list = list_create(free);

	// προσθήκη των αριθμών από το 0 έως 9 στη λίστα
	for (int i = 0; i < 10; i++) {
		list_insert_next(list, list_last(list), create_int(i));
	}
		
	for (ListNode node=list_first(list);
		node!=LIST_EOF;
		node=list_next(list, node)) {
			int* value = list_node_value(list, node);
			printf("%d\n", *value);
		}

	list_destroy(list);
	// εκτύπωση των περιεχομένων της λίστας
	//
	// for (...) {
	// 	...
	// }
} 