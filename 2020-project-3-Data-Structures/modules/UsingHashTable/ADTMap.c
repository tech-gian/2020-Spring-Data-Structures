/////////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT Map μέσω Hash Table με separate chaining
//
/////////////////////////////////////////////////////////////

#include <stdlib.h>

#include "ADTMap.h"
#include "ADTList.h"


// Οι κόμβοι του map περιέχουν το key και το value. Καθένας από αυτούς είναι το value ενός κόμβου λίστας.
// Το map δηλαδή αποτελείται από ένα array (στους κόμβους του οποίου hashaρουν τα keys), ο οποίας για
// στοιχεία του έχει λίστες για τα όλα τα keys που hashaρουν σε εκείνο το index.

// Το μέγεθος του Hash Table ιδανικά θέλουμε να είναι πρώτος αριθμός σύμφωνα με την θεωρία.
// Η παρακάτω λίστα περιέχει πρώτους οι οποίοι έχουν αποδεδιγμένα καλή συμπεριφορά ως μεγέθη.
// Κάθε re-hash θα γίνεται βάσει αυτής της λίστας. Αν χρειάζονται παραπάνω απο 1610612741 στοχεία, τότε σε καθε rehash διπλασιάζουμε το μέγεθος.
int prime_sizes[] = {53, 97, 193, 389, 769, 1543, 3079, 6151, 12289, 24593, 49157, 98317, 196613, 393241,
	786433, 1572869, 3145739, 6291469, 12582917, 25165843, 50331653, 100663319, 201326611, 402653189, 805306457, 1610612741};

// Χρησιμοποιούμε separate chaining, οπότε σύμφωνα με την θεωρία, πρέπει πάντα να διατηρούμε
// τον load factor του  hash table μικρότερο ή ίσο του 0.9, για να έχουμε αποδoτικές πράξεις
#define MAX_LOAD_FACTOR 0.9

// Δομή του κάθε κόμβου που έχει το hash table (με το οποίο υλοιποιούμε το map)
struct map_node{
	Pointer key;		// Το κλειδί που χρησιμοποιείται για να hash-αρουμε
	Pointer value;  	// Η τιμή που αντισοιχίζεται στο παραπάνω κλειδί
};

// Δομή του Map (περιέχει όλες τις πληροφορίες που χρεαζόμαστε για το HashTable)
struct map {
	List* array;				// Ο πίνακας που θα χρησιμοποιήσουμε για το map (remember, φτιάχνουμε ένα hash table)
	int capacity;				// Πόσο χώρο έχουμε δεσμεύσει.
	int size;					// Πόσα στοιχεία έχουμε προσθέσει
	CompareFunc compare;		// Συνάρτηση για σύγκριση δεικτών, που πρέπει να δίνεται απο τον χρήστη
	HashFunc hash_function;		// Συνάρτηση για να παίρνουμε το hash code του κάθε αντικειμένου.
	DestroyFunc destroy_key;	// Συναρτήσεις που καλούνται όταν διαγράφουμε έναν κόμβο απο το map.
	DestroyFunc destroy_value;
};


Map map_create(CompareFunc compare, DestroyFunc destroy_key, DestroyFunc destroy_value) {
	// Δεσμεύουμε κατάλληλα τον χώρο που χρειαζόμαστε για το hash table
	Map map = malloc(sizeof(*map));
	map->capacity = prime_sizes[0];
	map->array = malloc(map->capacity * sizeof(List));

	// Δημιουργούμε μια κενή list σε κάθε κόμβο του array
	for (int i = 0; i < map->capacity; i++)
		map->array[i] = list_create(free);

	map->size = 0;
	map->compare = compare;
	map->destroy_key = destroy_key;
	map->destroy_value = destroy_value;

	return map;
}

// Επιστρέφει τον αριθμό των entries του map σε μία χρονική στιγμή.
int map_size(Map map) {
	return map->size;
}

// Συνάρτηση για την επέκταση του Hash Table σε περίπτωση που ο load factor μεγαλώσει πολύ.
static void rehash(Map map) {
	// Αποθήκευση των παλιών δεδομένων
	int old_capacity = map->capacity;
	List* old_array = map->array;

	// Βρίσκουμε τη νέα χωρητικότητα, διασχίζοντας τη λίστα των πρώτων ώστε να βρούμε τον επόμενο. 
	int prime_no = sizeof(prime_sizes) / sizeof(int);	// το μέγεθος του πίνακα
	for (int i = 0; i < prime_no; i++) {					// LCOV_EXCL_LINE
		if (prime_sizes[i] > old_capacity) {
			map->capacity = prime_sizes[i]; 
			break;
		}
	}
	// Αν έχουμε εξαντλήσει όλους τους πρώτους, διπλασιάζουμε
	if (map->capacity == old_capacity)					// LCOV_EXCL_LINE
		map->capacity *= 2;								// LCOV_EXCL_LINE

	// Δεσμεύουμε χώρο για το νέο array και δημιουργούμε τις lists
	map->array = malloc(map->capacity * sizeof(List));
	for (int i=0 ; i<map->capacity ; i++)
		map->array[i] = list_create(free);

	map->size = 0;

	// Για κάθε MapNode που βρίσκεται στην κάθε list που υπάρχει στο old_array
	// εισάγουμε ξανάν τα key και value του στο map (με νέο hash)
	for (int i=0 ; i<old_capacity ; i++) {
		for (ListNode node=list_first(old_array[i]);
			node!=LIST_EOF;
			node=list_next(old_array[i], node)) {

			MapNode mnode = list_node_value(old_array[i], node);
			map_insert(map, mnode->key, mnode->value);
		}
	}

	for (int i=0 ; i<old_capacity ; i++)
		list_destroy(old_array[i]);

	free(old_array);

	//Αποδεσμεύουμε τον παλιό πίνακα ώστε να μήν έχουμε leaks
}

// Εισαγωγή στο hash table του ζευγαριού (key, item). Αν το key υπάρχει,
// ανανέωση του με ένα νέο value, και η συνάρτηση επιστρέφει true.

void map_insert(Map map, Pointer key, Pointer value) {
	// Βρίσκουμε το pos που πρέπει να μπει το key, αφού το hashaρουμε
	bool already_in_map = false;
	uint pos = map->hash_function(key) % map->capacity;
	MapNode mnode;

	// Στην list που υπάρχει στο αντίστοιχο array ψάχνουμε αν υπάρχει ήδη
	// το key που πρέπει να εισάγουμε
	for (ListNode node=list_first(map->array[pos]);
		node!=LIST_EOF;
		node=list_next(map->array[pos], node)) {
			mnode = list_node_value(map->array[pos], node);

			if (map->compare(mnode->key, key) == 0) {
				already_in_map = true;
				break;
			}
		}

	// Αν υπάρχει ήδη, γίνονται destroy τα key, value (αν υπάρχουν οι αντίστοιχες DestroyFunc)
	// και τα αντικαθιστούμε με τα νέα
	if (already_in_map) {
		if (mnode->key != key && map->destroy_key != NULL)
			map->destroy_key(mnode->key);
		if (mnode->value != value && map->destroy_value != NULL)
			map->destroy_value(mnode->value);

		mnode->key = key;
		mnode->value = value;
	}
	// Αλλιώς δημιουργείται νέο MapNode και γίνεται εισαγωγή του στο τέλος της list
	else {
		map->size++;
		MapNode new = malloc(sizeof(*new));
		new->key = key;
		new->value = value;
		list_insert_next(map->array[pos], list_last(map->array[pos]), new);
	}

	// Αν με την νέα εισαγωγή ξεπερνάμε το μέγιστο load factor, πρέπει να κάνουμε rehash
	float load_factor = (float)map->size / map->capacity;
	if (load_factor > MAX_LOAD_FACTOR)
		rehash(map);
}

// Διαγραφή απο το Hash Table του κλειδιού με τιμή key
bool map_remove(Map map, Pointer key) {
	// Βρίσκουμε το pos που hashaρει το key
	uint pos = map->hash_function(key) % map->capacity;
	ListNode temp = LIST_BOF;

	// Ψάχνουμε να βρούμε τον κόμβο της list που τα keys είναι ισοδύναμα
	for (ListNode node=list_first(map->array[pos]);
		node!=LIST_EOF;
		node=list_next(map->array[pos], node)) {

			MapNode mnode = list_node_value(map->array[pos], node);
			// Αν βρεθεί το key, κάνουμε destroy (αν υπάρχουν DestroyFunc)
			if (map->compare(key, mnode->key) == 0) {
				if (map->destroy_key != NULL)
					map->destroy_key(mnode->key);
				if (map->destroy_value != NULL)
					map->destroy_value(mnode->value);

				// και αφαιρούμε τον αντίστοιχο κόμβο της list
				list_remove_next(map->array[pos], temp);
				map->size--;
				return true;
			}
			temp = node;
		}

	// Αν δεν βρεθεί, γίνεται return false
	return false;
}

// Αναζήτηση στο map, με σκοπό να επιστραφεί το value του κλειδιού που περνάμε σαν όρισμα.

Pointer map_find(Map map, Pointer key) {
	MapNode node = map_find_node(map, key);
	if (node != MAP_EOF)
		return node->value;
	else
		return NULL;
}


DestroyFunc map_set_destroy_key(Map map, DestroyFunc destroy_key) {
	DestroyFunc old = map->destroy_key;
	map->destroy_key = destroy_key;
	return old;
}

DestroyFunc map_set_destroy_value(Map map, DestroyFunc destroy_value) {
	DestroyFunc old = map->destroy_value;
	map->destroy_value = destroy_value;
	return old;
}


// Βοηθητική συνάρτηση που κάνει destroy όλη την λίστα
// και τα keys, values που περιέχει κάθε κόμβος της

static void map_list_destroy(Map map, List list) {
	for (ListNode node=list_first(list);
		node!=LIST_EOF;
		node=list_next(list, node)) {
			MapNode mnode = list_node_value(list, node);

			if (map->destroy_key != NULL)
				map->destroy_key(mnode->key);
			if (map->destroy_value != NULL)
				map->destroy_value(mnode->value);
		}
	list_destroy(list);
}

// Απελευθέρωση μνήμης που δεσμεύει το map
void map_destroy(Map map) {
	// Για κάθε κόμβο του array, καλείται η map_list_destroy
	for (int i=0 ; i<map->capacity ; i++)
		map_list_destroy(map, map->array[i]);

	// Αποδέσμευση του array και του map
	free(map->array);
	free(map);
}

/////////////////////// Διάσχιση του map μέσω κόμβων ///////////////////////////

MapNode map_first(Map map) {
	//Ξεκινάμε την επανάληψή μας απο το 1ο στοιχείο, μέχρι να βρούμε κάτι όντως τοποθετημένο
	for (int i=0 ; i<map->capacity ; i++) {
		if (list_size(map->array[i]) != 0)
			return (MapNode) list_node_value(map->array[i], list_first(map->array[i]));
	}

	return MAP_EOF;
}

MapNode map_next(Map map, MapNode node) {
	// Βρίσκουμε το pos του key, άμα το hashaρουμε
	uint pos = map->hash_function(node->key) % map->capacity;

	// Ψάχνουμε τον node
	for (ListNode lnode=list_first(map->array[pos]);
		lnode!=LIST_EOF;
		lnode=list_next(map->array[pos], lnode)) {
			MapNode mnode = list_node_value(map->array[pos], lnode);

			// Αν βρεθεί τότε γίνεται return ο επόμενος κόμβος της list
			if (map->compare(mnode->key, node->key) == 0) {
				if (list_next(map->array[pos], lnode) != LIST_EOF)
					return (MapNode) list_node_value(map->array[pos], list_next(map->array[pos], lnode));
				// Αλλιώς το πρώτο στοιχείο από την επόμενη μη-κενή list του array
				else {
					pos = (pos+1)%map->capacity;
					for (int i=pos ; i<map->capacity ; i++) {
						if (list_size(map->array[i]) != 0)
							return (MapNode) list_node_value(map->array[i], list_first(map->array[i]));
					}
				}
			}
		}

	// Άμα δεν βρεθέι επόμενο MapNode, γίνεται return MAP_EOF
	return MAP_EOF;
}

Pointer map_node_key(Map map, MapNode node) {
	return node->key;
}

Pointer map_node_value(Map map, MapNode node) {
	return node->value;
}

MapNode map_find_node(Map map, Pointer key) {
	// Βρίσκουμε που hashaρει το key
	uint pos = map->hash_function(key) % map->capacity;
	MapNode mnode;

	// Στον αντίστοιχο κόμβο του array, ψάχνουμε να βρούμε ισοδύναμο key
	for (ListNode node=list_first(map->array[pos]);
		node!=LIST_EOF;
		node=list_next(map->array[pos], node)) {
			mnode = list_node_value(map->array[pos], node);

			// Άμα βρεθεί, γίνεται return
			if (map->compare(mnode->key, key) == 0)
				return mnode;
		}

	// Αλλιώς MAP_EOF
	return MAP_EOF;
}

// Αρχικοποίηση της συνάρτησης κατακερματισμού του συγκεκριμένου map.
void map_set_hash_function(Map map, HashFunc func) {
	map->hash_function = func;
}

uint hash_string(Pointer value) {
	// djb2 hash function, απλή, γρήγορη, και σε γενικές γραμμές αποδοτική
    uint hash = 5381;
    for (char* s = value; *s != '\0'; s++)
		hash = (hash << 5) + hash + *s;			// hash = (hash * 33) + *s. Το foo << 5 είναι γρηγορότερη εκδοχή του foo * 32.
    return hash;
}

uint hash_int(Pointer value) {
	return *(int*)value;
}

uint hash_pointer(Pointer value) {
	return (size_t)value;				// cast σε sizt_t, που έχει το ίδιο μήκος με έναν pointer
}