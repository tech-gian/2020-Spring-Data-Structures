///////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT Graph μέσω λιστών γειτνίασης.
//
///////////////////////////////////////////////////////////


#include <stdlib.h>
#include <limits.h>

#include "ADTGraph.h"

// Για την υλοποίηση της shortest_path
#include "ADTPriorityQueue.h"


// Το struct που χρησιμοποιείται για τον γράφο περιέχει ένα Map, στο οποίο η αντιστοίχηση
// key-value είναι vertex-λίστες γειτνίασης αντίστοιχα. Ακόμα, αποθηκεύεται η συνάρτηση
// compare και destroy_vertex. Δηλαδή, στο map, διατηρούμε καθεμία κορυφή και στην κάθε
// κορυφή αποθήκευται κι μια λίστα με τους γείτονες της και τα αντίστοιχα weights.

struct graph {
    Map map;                    // Ένα Map, με την υλοποίηση από την Άσκηση 1 (Separate-chaining)
    CompareFunc compare;        // Η συνάρτηση compare
};


// Το struct edge, είναι μία δομή στην οποία αποθηκεύουμε έναν γείτονα μαζί με το weight του
// για να εισαχθεί στην λίστα γειτνίασης 1 κορυφής.

struct edge {
    Pointer vertex;     // Η κορυφή του γείτονα
    uint weight;        // Το weight της ακμής
};
typedef struct edge* Edge;



Graph graph_create(CompareFunc compare, DestroyFunc destroy_vertex) {
    // Δέσμευση μνήμης για τον γράφο
    Graph graph = malloc(sizeof(*graph));

    // Αρχικοποίση των μελών του struct
    graph->map = map_create(compare, destroy_vertex, NULL);
    graph->compare = compare;
    
    return graph;
}


int graph_size(Graph graph) {
    // Το μέγεθος του γράφου είναι ουσιαστικα πόσες κορυφές έχει,
    // άρα το μέγεθος του map
    return map_size(graph->map);
}


void graph_insert_vertex(Graph graph, Pointer vertex) {
    // Δημιουργία λίστας-γειτνίασης
    List adj_list = list_create(free);
    // και εισαγωγή της κορυφής με την λίστα της στο map
    map_insert(graph->map, vertex, adj_list);
}


List graph_get_vertices(Graph graph) {
    // Δημιουργία λίστας (διαγράφεται από τον χρήστη)
    List vertices_list = list_create(NULL);

    // Εισαγωγή στην λίστα τις κορυφές του γράφου,
    // δηλαδή όλα τα keys του map
    for (MapNode node=map_first(graph->map);
        node!=MAP_EOF;
        node=map_next(graph->map, node)) {
            list_insert_next(vertices_list, list_last(vertices_list), map_node_key(graph->map, node));
        }

    return vertices_list;
}


void graph_remove_vertex(Graph graph, Pointer vertex) {
    // Εύρεση της λίστας γειτνίασης της κορυφής vertex
    List adj_list = map_find(graph->map, vertex);

    // Αν δεν υπάρχει η λίστα-γειτνίασης, τότε δεν υπάρχει κι η κορυφή vertex
    if (adj_list == NULL)
        return;

    // Αφαίρεση όλων των ακμών ανάμεσα στην κορυφή vertex
    // και τους γείτονες της, μέσω της λίστας-γειτνίασης
    int size = list_size(adj_list);
    for (int i=0 ; i<size ; i++) {
        Edge edge = list_node_value(adj_list, list_first(adj_list));
        graph_remove_edge(graph, vertex, edge->vertex);
    }

    // destroy της λίστας γειτνίασης
    list_destroy(adj_list);

    // remove την κορυφή vertex από το map
    map_remove(graph->map, vertex);
}


void graph_insert_edge(Graph graph, Pointer vertex1, Pointer vertex2, uint weight) {
    // Εύρεση της κορυφής και της λίστας-γειτνίασης του
    MapNode node = map_find_node(graph->map, vertex1);
    List adj_list = map_node_value(graph->map, node);

    // Δέσμευση μνήμης για το struct edge
    Edge edge = malloc(sizeof(*edge));
    edge->vertex = vertex2;
    edge->weight = weight;

    // Πρόσθεση στο τέλος της λίστας της νέας ακμής,
    // δηλαδή εισαγωγή του νέου γείτονα vertex2
    list_insert_next(adj_list, list_last(adj_list), edge);


    // Επαναλαμβάνεται η ίδια διαδικασία το vertex2
    node = map_find_node(graph->map, vertex2);
    adj_list = map_node_value(graph->map, node);

    edge = malloc(sizeof(*edge));
    edge->vertex = vertex1;
    edge->weight = weight;
    list_insert_next(adj_list, list_last(adj_list), edge);
}


void graph_remove_edge(Graph graph, Pointer vertex1, Pointer vertex2) {
    // Εύρεση της λίστας-γειτνίασης της κορυφής vertex1
    List adj_list = map_find(graph->map, vertex1);

    // Γίνεται remove από την λίστα-γειτνίασης του vertex1,
    // η κορυφή vertex2
    ListNode check_node = LIST_BOF;
    for (ListNode node=list_first(adj_list);
        node!=LIST_EOF;
        node=list_next(adj_list, node)) {
            Edge edge = list_node_value(adj_list, node);
            if (graph->compare(edge->vertex, vertex2) == 0) {
                list_remove_next(adj_list, check_node);
                break;
            }
            
            check_node = node;
        }

    // Επαναλαμβάνεται η ίδια διαδικασία για την κορυφή vertex2
    adj_list = map_find(graph->map, vertex2);

    check_node = LIST_BOF;
    for (ListNode node=list_first(adj_list);
        node!=LIST_EOF;
        node=list_next(adj_list, node)) {
            Edge edge = list_node_value(adj_list, node);
            if (graph->compare(edge->vertex, vertex1) == 0) {
                list_remove_next(adj_list, check_node);
                break;
            }

            check_node = node;
        }
}


uint graph_get_weight(Graph graph, Pointer vertex1, Pointer vertex2) {
    // Εύρεση της λίστας-γειτνίασης της κορυφής vertex1
    List adj_list = map_find(graph->map, vertex1);
    
    // Εύρεση του vertex2 σαν γείτονα του vertex1
    for (ListNode node=list_first(adj_list);
        node!=LIST_EOF;
        node=list_next(adj_list, node)) {
            Edge edge = list_node_value(adj_list, node);
            // Γίνεται return το weight τους
            if (graph->compare(vertex2, edge->vertex) == 0)
                return edge->weight;
        }

    // Αλλιώς το UINT_MAX
    return UINT_MAX;
}


List graph_get_adjacent(Graph graph, Pointer vertex) {
    // Εύρεση της λίστας-γειτνίασης της κορυφής vertex και δημιουργία μιας λίστας
    List ver_list = map_find(graph->map, vertex);
    List adj_list = list_create(NULL);

    // Στην adj_list, που θα γίνει return, εισάγονται
    // όλοι οι γείτονες του vertex1
    for (ListNode node=list_first(ver_list);
        node!=LIST_EOF;
        node=list_next(ver_list, node)) {
            Edge edge = list_node_value(ver_list, node);
            list_insert_next(adj_list, list_last(adj_list), edge->vertex);
        }

    // Αν δεν υπάρχουν γείτονες για το vertex1 τότε επιστρέφεται άδεια λίστα
    return adj_list;
}

// Δημιουργία ενός struct για την αποθήκευση όσων χρειαζόμαστε για το shortest_path

struct vertex {
    Pointer node;
    uint cost;
    Pointer prev;
    bool flag;
};
typedef struct vertex* Vertex;

// Κατάλληλη compare για την PriorityQueue

static int compare_pqueue(Pointer a, Pointer b) {
    uint cosa = ((Vertex)a)->cost;
    uint cosb = ((Vertex)b)->cost;
    
    if (cosa > cosb)
        return -1;
    else if (cosb > cosa)
        return 1;
    else
        return 0;
}


List graph_shortest_path(Graph graph, Pointer source, Pointer target) {
    // Η λίστα με όλες τις κορυφές του γράφου
    List ver = graph_get_vertices(graph);

    // Δημιουργία του pqueue και του map
    PriorityQueue pqueue = pqueue_create(compare_pqueue, NULL, NULL);
    Map vertices = map_create(graph->compare, NULL, NULL);
    map_set_hash_function(vertices, hash_pointer);

    // Το current Vertex
    Vertex testing;

    // Αρχικοποίηση του map και του pqueue με τα αντίστοιχα στοιχεία
    // από την λίστα ver
    for (ListNode node=list_first(ver);
        node!=LIST_EOF;
        node=list_next(ver, node)) {
            Vertex new = malloc(sizeof(*new));
            new->cost = UINT_MAX;
            new->node = list_node_value(ver, node);
            new->prev = NULL;
            new->flag = false;

            // Εισαγωγή στο pqueue του κόμβου Vertex
            PriorityQueueNode pq_node = pqueue_insert(pqueue, new);

            // Εισαγωγή στο map της κρουφής σαν key
            // και του PqueueNode σαν value
            map_insert(vertices, new->node, pq_node);

            // Όταν βρεθεί η source κορυφή βάζουμε τα στοιχεία που πρέπει
            // και μετά γίνεται pqueue_update_order
            if (graph->compare(new->node, source) == 0) {
                testing = new;
                testing->flag = true;
                testing->cost = 0;
                pqueue_update_order(pqueue, pq_node);
            }
        }

    // Ένας πίνακας που θα χρησιμοποιηθεί για την εύρεση της πιο κοντινής
    // κορυφής που δεν έχει ελεγχθεί
    Vertex* store = malloc(pqueue_size(pqueue) * sizeof(Vertex));

    // Το κύριο loop
    while (true) {
        // Παίρνουμε την λίστα με τους γείτονες
        List adj_list = graph_get_adjacent(graph, testing->node);

        // Για κάθε γείτονα
        for (ListNode node=list_first(adj_list);
            node!=LIST_EOF;
            node=list_next(adj_list, node)) {
                Pointer lnode = list_node_value(adj_list, node);

                // Βρίσκουμε τον Vertex κόμβο που αντιστοιχεί σε αυτήν την κορυφή
                PriorityQueueNode pq_node = map_find(vertices, lnode);
                Vertex temp = pqueue_node_value(pqueue, pq_node);

                // Το βάρος ανάμεσα στην testing (current) κορυφή και τον συγκεκριμένο γείτονα
                uint weight = graph_get_weight(graph, testing->node, temp->node);

                // Αν η απόσταση είναι πιο σύντομη από αυτή που υπάρχει ήδη στο Vertex
                // γίνονται οι κατάλληλες αλλαγές και pqueue_update_order
                if (testing->cost + weight < temp->cost) {
                    temp->cost = testing->cost + weight;
                    temp->prev = testing->node;
                    pqueue_update_order(pqueue, pq_node);
                }

            }

        // destroy της λίστας γειτόνων
        list_destroy(adj_list);

        // Αν φτάσαμε στον target, κλέινουμε το loop
        if (graph->compare(testing->node, target) == 0)
            break;

        // Βρίσκουμε τον επόμενο κοντινότερο, που δεν έχει ελεγχθεί
        // και έχει το μικρότερο cost
        int pos = 0;
        while (true) {
            // Θα είναι το pqueue_max
            Vertex node = pqueue_max(pqueue);

            // Αν δεν έχει ελεγχθεί, το παίρνουμε για να ξαναγίνει το loop
            if (node->flag == false) {
                node->flag = true;
                testing = node;
                for (int i=0 ; i<pos ; i++) {
                    PriorityQueueNode pq_node = pqueue_insert(pqueue, store[i]);
                    map_insert(vertices, store[i]->node, pq_node);                    
                }
                break;
            }
            // Αλλιώς το αφαιρούμε προσωρινά και το ξαναβάζουμε στο ακριβώς από πάνω for
            else {
                store[pos] = node;
                pqueue_remove_max(pqueue);
                map_remove(vertices, store[pos]->node);
                pos++;
            }
        }
    }

    // Δημιουργία της λίστας path
    List path = list_create(NULL);
    
    // Αν ο target κόμβος έχει cost==UINT_MAX
    // τότε δεν υπάρχει διαδρομή και επιστρέφουμε μια άδεια λίστα
    if (testing->cost != UINT_MAX) {
        // Αλλιώς εισάγουμε επαναληπτικά όλους του prev κόμβους
        // μέχρι να φτάσουμε στον source
        while (true) {
            list_insert_next(path, LIST_BOF, testing->node);
            if (graph->compare(testing->node, source) == 0)
                break;
            
            Vertex temp = pqueue_node_value(pqueue, (PriorityQueueNode)map_find(vertices, testing->prev));
            testing = temp;
        }
    }

    // Τα απαραίτητα destroy για τις δομές που χρησιμοποιήθηκαν
    map_destroy(vertices);

    pqueue_set_destroy_value(pqueue, free);
    pqueue_destroy(pqueue);

    free(store);
    list_destroy(ver);

    // Επιστροφή της λίστας
    return path;
}


void graph_destroy(Graph graph) {
    // Για κάθε κορυφή γίνεται destroy η λίστα-γειτνίασης
    for(MapNode node=map_first(graph->map);
        node!=MAP_EOF;
        node=map_next(graph->map, node)) {
            List adj_list = map_node_value(graph->map, node);
            list_destroy(adj_list);
        }

    // Αφού για destroy_key έχει περαστεί η destroy_vertex
    // κατά το map_destroy, θα γίνουν destroy και οι κορυφές
    map_destroy(graph->map);

    // Αποδέσμευση μνήμης του struct του γράφου
    free(graph);
}



//// Για την περίπτωση που ο γράφος χρησιμοποιεί πίνακα κατακερματισμού

void graph_set_hash_function(Graph graph, HashFunc hash_func) {
    // Ορίζει την hash_function στο map
    map_set_hash_function(graph->map, hash_func);
}
