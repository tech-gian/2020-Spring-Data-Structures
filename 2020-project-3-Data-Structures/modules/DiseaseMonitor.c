///////////////////////////////////
// 
// Υλοποίηση του DiseaseMonitor
// 
// Οι λειτουργίες του περιγράφονται
// στο DiseaseMonitor.h
// 
///////////////////////////////////



// Βασικές βιβλιοθήκες που χρειάζονται

#include <stdlib.h>
#include <string.h>
#include <limits.h>

// header file, για τις συναρτήσεις

#include "DiseaseMonitor.h"

// headers files, για τις συμπληρωματικές δομές

#include "ADTMap.h"
#include "ADTSet.h"
#include "ADTPriorityQueue.h"


////////////////////////////////////////
// 
// structs που χρησιμοποιούνται παρακάτω
////////////////////////////////////////


// struct που αποθηκεύει ένα map και ένα priority_queue.
// Χρησιμοποιείται στην dm_top_diseases
struct map_pq {
    Map dis_map;            // Map με key->disease, value->PQNode
    PriorityQueue pq_top;   // Pqueue με compare για τον αριθμό των κρουσμάτων
};
typedef struct map_pq* MapPq;



struct date_map {
    Date date;              // Ημερομηνία (το DateMap είναι value για το Set)
    Map rec_map;            // Map με όλα τα κρούσματα εκείνου του date
};
typedef struct date_map* DateMap;


// struct που αποθηκεύει ένα disease και τον αριθμό κρουσμάτων.
// Χρησιμοποιείται ως value στο pqueue του MapPq
struct top_dis {
    String disease;         // Ονομασία disease (το TopDis είναι value για το Pqueue)
    int k;                  // Αριθμός κρουσμάτων
};
typedef struct top_dis* TopDis;



// struct που αποθηκεύει 3 maps και ένα MapPq.
// Χρησιμοποιείται σε όλες τις λειτουργίες.
// Είναι το disease_monitor, που διατηρεί όλα τα
// records, με διάφορες διατάξεις (ανάλογα πως θέλουμε
// να τα διαχειριστούμε)
struct disease_monitor {
    Map rec;            // Map με όλα τα records, key->id value->record
    Map dis_c_date;     // Map με key->disease και value->Map με key-> country
                        // και value->Set με value DateMap
    Map country_top;    // Map με key->country και value->MapPq
    MapPq country_null; // MapPq, για όταν στην top_diseases δοθεί country==NULL
};
typedef struct disease_monitor* DisMon;


// Αρχικοποίηση σε NULL, αφού είναι global variable
// και χρησιμοποιείται από όλες τις λειτουργίες
static DisMon dm_main = NULL;



//////////////////////
// Συναρτήσεις compare

int compare_records(Pointer a, Pointer b) {
    return *(int*)a - *(int*)b;
}

int compare_dis(Pointer a, Pointer b) {
    return strcmp((String)a, (String)b);
}

int compare_country(Pointer a, Pointer b) {
    return strcmp((String)a, (String)b);
}

int compare_date(Pointer a, Pointer b) {
    return strcmp(((DateMap)a)->date, ((DateMap)b)->date);
}

int compare_pq_top(Pointer a, Pointer b) {
    return ((TopDis)a)->k - ((TopDis)b)->k;
}



// Λειτουργίες του disease_monitor

void dm_init(void) {
    // Αν υπάρχει ήδη κάποιο disease_monitor,
    // τότε γίνεται destroy και δημιουργείται νέο
    if (dm_main != NULL)
        dm_destroy();

    // Δέσμευση μνήμης για την βασικό struct
    dm_main = malloc(sizeof(*dm_main));


    // Map που διατηρεί όλα τα records, με βάση το id
    // (key->id, value->record)
    // (Υπάρχει και αυτό το Map για γρήγορη αναζήτηση στην dm_remove_record)
    dm_main->rec = map_create(compare_records, NULL, NULL);
    map_set_hash_function(dm_main->rec, hash_int);

    // Map που διατηρεί ένα Map για κάθε disease
    // (key->disease, value->Map)
    dm_main->dis_c_date = map_create(compare_dis, NULL, NULL);
    map_set_hash_function(dm_main->dis_c_date, hash_string);

    // Map που διατηρεί ένα MapPq για κάθε country
    // (key->country, value->MapPq)
    dm_main->country_top = map_create(compare_country, NULL, NULL);
    map_set_hash_function(dm_main->country_top, hash_string);

    // MapPq που διατηρεί ένα Map και ένα Pqueue.
    // Το Map του MapPq (key->disease, value->PQNode)
    dm_main->country_null = malloc(sizeof(*(dm_main->country_null)));

    // Αρχικοποίηση των μελών του MapPq
    MapPq map_pq = dm_main->country_null;
    map_pq->dis_map = map_create(compare_dis, NULL, NULL);
    map_set_hash_function(map_pq->dis_map, hash_string);
    map_pq->pq_top = pqueue_create(compare_pq_top, free, NULL);
}


void dm_destroy(void) {
    // destroy το Map, που αντιστοιχούν τα records (αυτά γίνονται destroy από τον χρήστη)
    map_destroy(dm_main->rec);

    // Προσπέλαση κάθε MapNode του dis_c_date
    for (MapNode node=map_first(dm_main->dis_c_date);
        node!=MAP_EOF;
        node=map_next(dm_main->dis_c_date, node)) {
            Map disease = map_node_value(dm_main->dis_c_date, node);

            // Προσπέλαση κάθε MapNode του disease
            for (MapNode mnode=map_first(disease);
                mnode!=MAP_EOF;
                mnode=map_next(disease, mnode)) {
                    Set country = map_node_value(disease, mnode);

                    // Προσπέλαση κάθε SetNode του country
                    for (SetNode snode=set_first(country);
                        snode!=SET_EOF;
                        snode=set_next(country, snode)) {
                            // Αυτό γίνεται free, αφού έχει περαστεί η free
                            // ως DestroyFunc στο Set
                            DateMap temp = set_node_value(country, snode);

                            // destroy του map, που βρίσκεται σε κάθε DateMap
                            map_destroy(temp->rec_map);
                        }
                    // destroy το Set counrty
                    set_destroy(country);
                }
            // destroy το Map disease
            map_destroy(disease);
        }


    // destroy το Map, που έχει σαν values τα αντίστοιχα Maps
    map_destroy(dm_main->dis_c_date);


    // Προσπέλαση κάθε MapNode του country_top
    for (MapNode node=map_first(dm_main->country_top);
        node!=MAP_EOF;
        node=map_next(dm_main->country_top, node)) {
            MapPq map_pq = map_node_value(dm_main->country_top, node);

            // destroy το Map και το pqueue του MapPq
            map_destroy(map_pq->dis_map);
            pqueue_destroy(map_pq->pq_top);

            // Αποδέσμευση μνήμης για το MapPq
            free(map_pq);
        }

    // destroy το Map country_top
    map_destroy(dm_main->country_top);

    // destroy του Map και του pqueue του MapPq country_null
    map_destroy(dm_main->country_null->dis_map);
    pqueue_destroy(dm_main->country_null->pq_top);

    // Αποδέσμευση μνήμης για το MapPq
    free(dm_main->country_null);


    // Αποδέσμευση μνήμης του "βασικού" struct
    free(dm_main);
    // Το κάνουμε ίσο με NULL, για να ελεγχθεί στην dm_init
    dm_main = NULL;
}


bool dm_insert_record(Record record) {
    // Αν δεν υπάρχει εγγραφή με ίδιο id, γίνεται return false
    bool flag = false;

    // Αν υπάρχει εγγραφή με ίδιο id, αντικαθίσταται
    if (map_find(dm_main->rec, &(record->id)) != NULL)
        // Γίνεται return true
        flag = true;

    // προσθέτουμε το record (αντικαθιστούμε αν υπάρχει ίδιο)
    map_insert(dm_main->rec, &(record->id), record);

    // Ακολουθείται η ίδια διαδικασία στο Map dis_c_date

    // Βρίσκουμε (αν όχι, προσθέτουμε) την disease σαν key με το αντίστοιχο Map σαν value
    Map disease;
    if ((disease=map_find(dm_main->dis_c_date, record->disease)) == NULL) {
        disease = map_create(compare_country, NULL, NULL);
        map_set_hash_function(disease, hash_string);
        map_insert(dm_main->dis_c_date, record->disease, disease);
    }

    // Βρίσκουμε (αν όχι, προσθέτουμε) την country σαν key με το αντίστοιχο Set σαν value
    Set country;
    if ((country=map_find(disease, record->country)) == NULL) {
        country = set_create(compare_date, free);
        map_insert(disease, record->country, country);
    }

    // Στο Set αυτό (Set country) εισάγουμε DateMap,
    // το οποίο είναι struct που διατηρεί την συγκεκριμένη ημερομηνία
    // και ένα Map με όλα τα records που αφορούν εκείνη την ημερομηνία.
    // (προφανώς για το disease και country που βρισκόμαστε)

    DateMap date_map;
    // Δημιουργείται για να βρεθεί το DateMap με την συγκεκριμένη ημερομηνία
    DateMap temp = malloc(sizeof(*temp));
    temp->date = record->date;

    // Βρίσκουμε (αν όχι, προσθέτουμε) την date, με δημιουργία Map
    if ((date_map=set_find(country, temp)) == NULL) {
        date_map = malloc(sizeof(*date_map));
        date_map->date = record->date;
        date_map->rec_map = map_create(compare_records, NULL, NULL);
        map_set_hash_function(date_map->rec_map, hash_int);
        set_insert(country, date_map);
    }

    // Εισαγωγή στο Map του id σαν key και του record σαν value
    map_insert(date_map->rec_map, &(record->id), record);


    // Για την τελευταία συνάρτηση χρειαζόμαστε 2 διαφορετικά "δομές"
    // ώστε να δημιουργηθεί γρήγορα η λίστα

    // Βρίσκουμε (αν όχι, προσθέτουμε) στο Map country_top
    // σαν key το country και σαν value ένα MapPq.
    // To MapPq περιέχει ένα Map (key->disease, value->PQNode)
    // και ένα pqueue (με στοιχεία TopDis)
    MapPq map_pq = map_find(dm_main->country_top, record->country);
    if (map_pq == NULL) {
        map_pq = malloc(sizeof(*map_pq));
        map_pq->dis_map = map_create(compare_dis, NULL, NULL);
        map_set_hash_function(map_pq->dis_map, hash_string);
        map_pq->pq_top = pqueue_create(compare_pq_top, free, NULL);
        map_insert(dm_main->country_top, record->country, map_pq);
    }
    
    // Σε κάθε TopDis αποθηκεύεται το disease και ο αριθμός των κρουσμάτων

    // Αν βρεθεί, απλά ανανεώνεται
    // Αλλιώς, δημιουργείται και αρχικοποιείται το k=1
    PriorityQueueNode pq_node = map_find(map_pq->dis_map, record->disease);
    if (pq_node == NULL) {
        TopDis top_dis = malloc(sizeof(*top_dis));
        top_dis->disease = record->disease;
        top_dis->k = 1;
        PriorityQueueNode pq_node = pqueue_insert(map_pq->pq_top, top_dis);
        map_insert(map_pq->dis_map, record->disease, pq_node);
    }
    else {
        TopDis top_dis = pqueue_node_value(map_pq->pq_top, pq_node);
        top_dis->k++;
        pqueue_update_order(map_pq->pq_top, pq_node);
    }


    // Γίνεται η ίδια διαδικασία το MapPq, απλά αυτή τη φορά
    // δεν αποτελεί value σε κάποιο Map, αλλα υπάρχει στην dm_main.
    // Κι αυτό γιατί έχει διατεταγμένες ανά αριθμό κρουσμάτων όλες τις ασθένειες,
    // ανεξαρτήτου χώρας. (Θα χρησιμοποιηθεί όταν δοθεί country=NULL)
    map_pq = dm_main->country_null;
    pq_node = map_find(map_pq->dis_map, record->disease);
    if (pq_node == NULL) {
        TopDis top_dis = malloc(sizeof(*top_dis));
        top_dis->disease = record->disease;
        top_dis->k = 1;
        PriorityQueueNode pq_node = pqueue_insert(map_pq->pq_top, top_dis);
        map_insert(map_pq->dis_map, record->disease, pq_node);
    }
    else {
        TopDis top_dis = pqueue_node_value(map_pq->pq_top, pq_node);
        top_dis->k++;
        pqueue_update_order(map_pq->pq_top, pq_node);
    }
    


    free(temp);

    return flag;
}


bool dm_remove_record(int id) {
    // Βρίσκουμε (αν όχι, return false), το Record target,
    // το οποίο είναι για αφαίρεση, με βάση το id
    Record target = map_find(dm_main->rec, &(id));
    if (target == NULL)
        return false;

    // Βρίσκουμε το target στο Map dis_c_date
    Map disease = map_find(dm_main->dis_c_date, target->disease);

    // Βρίσκουμε το target στο Map disease
    Set country = map_find(disease, target->country);

    // Βρίσκουμε το target στο Set counrty
    DateMap temp = malloc(sizeof(*temp));
    temp->date = target->date;
    DateMap target_temp = set_find(country, temp);

    free(temp);


    // Γίνεται το remove από το Map, που περιέχεται στο Set country
    map_remove(target_temp->rec_map, &(id));
    // Γίνεται το remove το Map records
    map_remove(dm_main->rec, &(id));


    // Βρίσκουμε το MapPq
    MapPq map_pq = map_find(dm_main->country_top, target->country);
    PriorityQueueNode pq_node = map_find(map_pq->dis_map, target->disease);
    // Βρίσκουμε το TopDis από το pqueue (αφού βρήκαμε το PQNode από το map_pq->dis_map)
    TopDis top_dis = pqueue_node_value(map_pq->pq_top, pq_node);
    
    // Γίνονται οι απαραίτητες αλλαγές και ανανεώνουμε την pqueue
    top_dis->k--;
    pqueue_update_order(map_pq->pq_top, pq_node);


    // Αντίστοιχη διαδικασία γίνεται για το MapPq country_null
    map_pq = dm_main->country_null;
    pq_node = map_find(map_pq->dis_map, target->disease);
    top_dis = pqueue_node_value(map_pq->pq_top, pq_node);
    top_dis->k--;
    pqueue_update_order(map_pq->pq_top, pq_node);

    return true;
}

// Βοηθητική Συνάρτηση

static void dm_get_set(Date date_from, Date date_to, List rec, Set country_set) {
// Σε αυτή τη συνάρτηση ψάχνουμε ανάλογα το date_from και το date_to στο Set
// τα αντίστοιχα records που αντιστοιχούν στο εύρος ημερομηνιών

    // Αρχικοποιήσεις για να βρεθούν τα αντίστοιχα SetNodes
    DateMap date1 = malloc(sizeof(*date1));
    DateMap date2 = malloc(sizeof(*date2));
    date1->date = date_from;
    date2->date = date_to;

    // Αν δεν δοθεί ούτε date_from ούτε date_to
    if (date_from == NULL && date_to == NULL) {
        
        // Επιστρέφουμε τα records που υπάρχουν σε όλο το Set
        for (SetNode node=set_first(country_set);
            node!=SET_EOF;
            node=set_next(country_set, node)) {
                DateMap temp = set_node_value(country_set, node);
                

                for (MapNode mnode=map_first(temp->rec_map);
                    mnode!=MAP_EOF;
                    mnode=map_next(temp->rec_map, mnode)) {

                        list_insert_next(rec, list_last(rec), map_node_value(temp->rec_map, mnode));
                    }
            }
    }
    // Αν δοθεί μόνο date_to
    else if (date_from == NULL) {
        SetNode end = set_find_node(country_set, date2);
        // Ψάχνει μέχρι και τον end
        if (end != SET_EOF)
            end = set_next(country_set, end);
        
        // Αν το date_to δεν υπάρχει, τότε με την strcmp,
        // γίνεται έλεγχος αν ξεπεράσαμε την συγκεκριμένη ημερομηνία

        // Επιστρέφουμε τα records που υπάρχουν από την αρχή του Set
        // μέχρι τον κόμβο end
        for (SetNode node=set_first(country_set);
            node!=end;
            node=set_next(country_set, node)) {
                DateMap temp = set_node_value(country_set, node);
                if (strcmp(temp->date, date_to) > 0)
                    break;

                for (MapNode mnode=map_first(temp->rec_map);
                    mnode!=MAP_EOF;
                    mnode=map_next(temp->rec_map, mnode)) {

                        list_insert_next(rec, list_last(rec), map_node_value(temp->rec_map, mnode));
                    }
            }
    }
    // Αν δοθεί μόνο date_from
    else if (date_to == NULL) {
        SetNode start = set_find_node(country_set, date1);
        // Αν το date_from δεν υπάρχει, τότε πρέπει να το κάνουμε ίσο
        // με το set_first και με την strcmp, γίνεται έλεγχος αν βρισκόμαστε
        // εντός του εύρος τιμών
        if (start == SET_EOF)
            start = set_first(country_set);

        // Επιστρέφουμε τα records που υπάρχουν από τον κόμβο start,
        // μέχρι το τέλος του Set
        for (SetNode node=start;
            node!=SET_EOF;
            node=set_next(country_set, node)) {
                DateMap temp = set_node_value(country_set, node);

                if (strcmp(temp->date, date_from) >= 0) {
                    for (MapNode mnode=map_first(temp->rec_map);
                        mnode!=MAP_EOF;
                        mnode=map_next(temp->rec_map, mnode)) {

                            list_insert_next(rec, list_last(rec), map_node_value(temp->rec_map, mnode));
                        }
                }
            }
    }
    // Αν δοθούν και date_from και date_to
    else {
        SetNode start = set_find_node(country_set, date1);
        SetNode end = set_find_node(country_set, date2);

        // Αν το date_from δεν υπάρχει, τότε πρέπει να το κάνουμε ίσο
        // με το set_first και με την strcmp, γίνεται έλεγχος αν βρισκόμαστε
        // εντός του εύρος τιμών
        if (start == SET_EOF)
            start = set_first(country_set);

        // Ψάχνει μέχρι και τον end
        if (end != SET_EOF)
            end = set_next(country_set, end);

        // Αν το date_to δεν υπάρχει, τότε με την strcmp,
        // γίνεται έλεγχος αν ξεπεράσαμε την συγκεκριμένη ημερομηνία

        // Επιστρέφουμε τα records που υπάρχουν από τον κόμβο start,
        // μέχρι τον κόμβο end
        for (SetNode node=start;
            node!=end;
            node=set_next(country_set, node)) {
                DateMap temp = set_node_value(country_set, node);
                
                if (strcmp(temp->date, date_to) > 0)
                    break;

                if (strcmp(temp->date, date_from) >= 0) {
                    for (MapNode mnode=map_first(temp->rec_map);
                        mnode!=MAP_EOF;
                        mnode=map_next(temp->rec_map, mnode)) {

                            list_insert_next(rec, list_last(rec), map_node_value(temp->rec_map, mnode));
                        }
                }
            }
    }

    free(date1);
    free(date2);
}


List dm_get_records(String disease, String country, Date date_from, Date date_to) {
    // Δημιουργία λίστας
    List rec = list_create(NULL);

    // Αν δεν δοθεί disease, τις ψάχνουμε όλες
    if (disease == NULL) {
        for (MapNode nodem=map_first(dm_main->dis_c_date);
            nodem!=MAP_EOF;
            nodem=map_next(dm_main->dis_c_date, nodem)) {
                Map disease_map = map_node_value(dm_main->dis_c_date, nodem);

                // Αν δεν δοθεί country, τις ψάχνουμε όλες
                if (country == NULL) {
                    for (MapNode node_map=map_first(disease_map);   
                        node_map!=MAP_EOF;
                        node_map=map_next(disease_map, node_map)) {
                            
                            Set country_set = map_node_value(disease_map, node_map);
                            // Καλούμε την dm_get_set για τις ημερομηνίες
                            dm_get_set(date_from, date_to, rec, country_set);
                        }
                }
                // Αλλιώς βρίσκουμε την country που μας δίνεται
                else {
                    Set country_set = map_find(disease_map, country);
                    if (country_set != NULL)
                        dm_get_set(date_from, date_to, rec, country_set);
                }
            }
    }
    // Αλλιώς βρίσκουμε την disease που μας δίνεται
    else {
        Map disease_map = map_find(dm_main->dis_c_date, disease);
        if (disease_map == NULL)
            return rec;

        // Αν δεν δοθεί country, τις ψάχνουμε όλες
        if (country == NULL) {
            for (MapNode node_map=map_first(disease_map);
                node_map!=MAP_EOF;
                node_map=map_next(disease_map, node_map)) {

                    Set country_set = map_node_value(disease_map, node_map);
                    dm_get_set(date_from, date_to, rec, country_set);

                }
        }
        // Αλλιώς βρίσκουμε την country που μας δίνεται
        else {
            Set country_set = map_find(disease_map, country);
            if (country_set != NULL)
                dm_get_set(date_from, date_to, rec, country_set);
        }
    }

    // Επιστρέφουμε την λίστα
    return rec;
}

// Βοηθητική Συνάρτηση

static void dm_count_set(Date date_from, Date date_to, int* size, Set country_set) {
// Σε αυτή τη συνάρτηση ψάχνουμε ανάλογα το date_from και το date_to στο Set
// τα αντίστοιχα records που αντιστοιχούν στο εύρος ημερομηνιών και επιστρέφουμε το size

    // Αρχικοποιήσεις για να βρεθούν τα αντίστοιχα SetNodes
    DateMap date1 = malloc(sizeof(*date1));
    DateMap date2 = malloc(sizeof(*date2));
    date1->date = date_from;
    date2->date = date_to;

    // Αν δεν δοθεί ούτε date_from ούτε date_to
    if (date_from == NULL && date_to == NULL) {
        for (SetNode node=set_first(country_set);
            node!=SET_EOF;
            node=set_next(country_set, node)) {
                DateMap temp = set_node_value(country_set, node);
                
                // Επιστρέφουμε το άθροισμα των map_size που υπάρχουν
                // σε όλα τα DateMap του Set
                *size += map_size(temp->rec_map);
            }

    }
    // Αν δοθεί μόνο date_to
    else if (date_from == NULL) {
        SetNode end = set_find_node(country_set, date2);

        // Αν το end δεν είναι SET_EOF, τότε ψάχνουμε
        // μέχρι και τον κόμβο end
        if (end != SET_EOF)
            end = set_next(country_set, end);
        

        // Αν το end είναι SET_EOF, τότε δεν υπάρχει η συγκεκριμένη ημερομηνία,
        // οπότε με την strcmp σταματάμε αμα την ξεπεράσουμε
        for (SetNode node=set_first(country_set);
            node!=end;
            node=set_next(country_set, node)) {
                DateMap temp = set_node_value(country_set, node);
                if (strcmp(temp->date, date_to) > 0)
                    break;

                // Επιστρέφουμε το άθροισμα των map_size που υπάρχουν
                // σε όλα τα DateMap του Set
                *size += map_size(temp->rec_map);
            }
    }
    // Αν δοθεί μόνο date_from
    else if (date_to == NULL) {
        SetNode start = set_find_node(country_set, date1);
        
        // Αν το start είναι SET_EOF, τότε του βάζουμε το set_first
        if (start == SET_EOF)
            start = set_first(country_set);

        for (SetNode node=start;
            node!=SET_EOF;
            node=set_next(country_set, node)) {
                DateMap temp = set_node_value(country_set, node);
                
                // Με την strcmp υπολογίζουμε μόνο τις ημερομηνίες
                // που είναι μεγαλύτερες από το date_from
                if (strcmp(temp->date, date_from) >= 0)
                    *size += map_size(temp->rec_map);
            }
    }
    // Αν δοθούν και date_from και date_to
    else {
        SetNode start = set_find_node(country_set, date1);
        SetNode end = set_find_node(country_set, date2);

        // Αν το start είναι SET_EOF του βάζουμε το set_first
        if (start == SET_EOF)
            start = set_first(country_set);

        // Φτιάχνουμε το end, ώστε να ψάχνουμε μέχρι και αυτό
        if (end != SET_EOF)
            end = set_next(country_set, end);

        for (SetNode node=start;
            node!=end;
            node=set_next(country_set, node)) {
                DateMap temp = set_node_value(country_set, node);

                // Αν ξεπεράσουμε την date_to, break
                if (strcmp(temp->date, date_to) > 0)
                    break;
                
                // Με την strcmp υπολογίζουμε μόνο τις ημερομηνίες
                // που είναι μεγαλύτερες από το date_from
                if (strcmp(temp->date, date_from) >= 0)
                    *size += map_size(temp->rec_map);
            }
    }

    free(date1);
    free(date2);
}



int dm_count_records(String disease, String country, Date date_from, Date date_to) {
    // Αρχικοποίηση του size, που θα αποθηκεύεται ο αριθμός
    // των records που πλρούν τις προυποθέσεις
    int size = 0;

    // Αν δεν δοθεί disease, τις ψάχνουμε όλες
    if (disease == NULL) {
        for (MapNode node=map_first(dm_main->dis_c_date);
            node!=MAP_EOF;
            node=map_next(dm_main->dis_c_date, node)) {
                Map disease_map = map_find(dm_main->dis_c_date, (String)map_node_key(dm_main->dis_c_date, node));

                // Αν δεν δοθεί country, τις ψάχνουμε όλες
                if (country == NULL) {
                    for (MapNode node=map_first(disease_map);
                        node!=MAP_EOF;
                        node=map_next(disease_map, node)) {
                            Set country_set = map_node_value(disease_map, node);
                            dm_count_set(date_from, date_to, &size, country_set);
                        }
                }
                // Αλλιώς βρίσκουμε το country που μας δίνεται
                else {
                    Set country_set = map_find(disease_map, country);

                    if (country_set != NULL)
                        dm_count_set(date_from, date_to, &size, country_set);
                }
            }
    }
    // Αλλιώς βρίσκουμε την disease που μας δίνεται
    else {
        Map disease_map = map_find(dm_main->dis_c_date, disease);
        if (disease_map == NULL)
            return size;

        // Αν δεν δοθεί country, τις ψάχνουμε όλες
        if (country == NULL) {
            for (MapNode node=map_first(disease_map);
                node!=MAP_EOF;
                node=map_next(disease_map, node)) {
                    Set country_set = map_node_value(disease_map, node);
                    dm_count_set(date_from, date_to, &size, country_set);
                }
        }
        // Αλλιώς βρίσκουμε την country που μας δίνεται
        else {
            Set country_set = map_find(disease_map, country);

            if (country_set == NULL)
                return size;
            else
                dm_count_set(date_from, date_to, &size, country_set);
        }
    }
    
    // Επιστρέφουμε τον αριθμό των records
    return size;
}


List dm_top_diseases(int k, String country) {
    // Δημιουργία της λίστας
    List top = list_create(NULL);

    // Αν δεν δοθεί country, τότε χρησιμοποιείται η dm_main->country_null
    if (country == NULL) {
        TopDis array[k];
        PriorityQueue pqueue = dm_main->country_null->pq_top;

        // Για να μην γίνουν destroy κατά την pqueue_remove_max
        DestroyFunc old = pqueue_set_destroy_value(pqueue, NULL);
        
        // Βάζουμε στην λίστα οι k diseases που έχουν τα περισσότερα
        // κρούσματα (με την pqueue_max) και μετά αφαιρούνται για
        // να βρούμε και το επόμενο
        for (int i=0 ; i<k ; i++) {
            array[i] = pqueue_max(pqueue);
            list_insert_next(top, list_last(top), array[i]->disease);
            pqueue_remove_max(pqueue);
        }
        // Μετά τα ξαναπροσθέτουμε για μελλοντική χρήση
        for (int i=0 ; i<k ; i++)
            pqueue_insert(pqueue, array[i]);

        pqueue_set_destroy_value(pqueue, old);
    }
    // Αν δοθεί country
    else {
        // Βρίσκουμε το country
        MapPq map_pq = map_find(dm_main->country_top, country);
        if (map_pq != NULL) {
            TopDis array[k];
            PriorityQueue pqueue = map_pq->pq_top;

            // Για να μην γίνουν destroy κατά την pqueue_remove_max
            DestroyFunc old = pqueue_set_destroy_value(pqueue, NULL);
            
            // Και εκτελούμε την ίδια διαδικασία
            for (int i=0 ; i<k ; i++) {
                array[i] = pqueue_max(pqueue);
                list_insert_next(top, list_last(top), array[i]->disease);
                pqueue_remove_max(pqueue);
            }
            for (int i=0 ; i<k ; i++)
                pqueue_insert(pqueue, array[i]);

            pqueue_set_destroy_value(pqueue, old);
        }
    }

    // Επιστρέφουμε την λίστα
    return top;
}
