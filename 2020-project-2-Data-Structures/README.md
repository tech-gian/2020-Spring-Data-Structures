![run-tests](../../workflows/run-tests/badge.svg)

## 2020 Project 2

Εκφώνηση: https://k08.chatzi.org/projects/project2/


### Προσωπικά στοιχεία

__Όνομα__: Ιωάννης Ζαπάντης

__Α.Μ.__: sdi1900059


### Documentation


# Άσκηση 1:
    Σε αυτήν την άσκηση προστέθηκε η λειτουργία
    vector_steps, η οποία επιστρέφει πόσα βήματα
    πραγματοποίησε η τελευταία λειτουργία.

    Όσες λειτουργίες έχουν σταθερό αριθμό βημάτων
    (ανεξάρτητο του n) θεωρώ ότι κάνουν 1 βήμα.
    Στις υπόλοιπες υπολογίζω τον ακριβή αριθμό
    βημάτων που εκτελούνται.

    Το πρόγραμμα vector_insert_steps βρίσκεται
    στο folder programs/vector_insert_steps και
    έχει το .c αρχείο και ένα Makefile, το οποίο
    έχει τις απλές λειτουργίες make, run, clean
    και valgrind. Το πρόγραμμα δεν εμφανίζει
    memory leaks. Στο Makefile, σαν ARGS πρέπει
    να δοθεί είτε "real" είτε "amortized".

    Οι γραφικές παραστάσεις βρίσκονται στον folder
    images και παρατίθενται στο τέλος του README.


# Άσκηση 2:
    Σε αυτήν την άσκηση, υλοποιήθηκε το module για
    την Bidirectional List. Οι διαφορές της με την
    απλή λίστα αναφέρονται στην εκφώνηση της άσκησης.

    Υλοποιήθηκαν όλες οι λειτουργίες και έχουν όλες
    πολυπλοκότητα O(1), εκτός από την blist_find
    (που ουσιαστικά καλεί την blist_find_node), την
    blist_find_node και την blist_destroy που έχουν
    πολυπλοκότητα O(n), αφού πρέπει σε κάθε μία από τις
    παραπάνω περιπτώσεις να γίνει διάσχιση όλης της λίστας.

    Μια σημαντική διαφορά με την απλή λίστα είναι ότι
    χρησιμοποίησα τον dummy κόμβο στο τέλος αντί για την
    αρχή της λίστας. Έτσι απλοποιήθηκαν κάποιοι αλγόριθμοι, 
    όπως η blist_insert, αφού εισάγει κάποιον κόμβο πριν
    από τον node. Αντίστοιχα, έχει αλλάξει η blist_first,
    η blist_last και οι υπόλοιπες που χρησιμοποιούν τον
    dummy, χωρίς όμως να έχει αλλάξει η λογική, με την οποία
    χρησιμοποιείται.

    
    Ακόμα, γράφτηκαν tests για τον έλεγχο των λειτουργιών.
    Βρίσκονται στο folder tests, μέσα στο αρχείο
    ADTBList_test.c. Όλα τα test, περνάνε και χωρίς leaks,
    με την χρήση του Makefile, το οποίο ήταν ήδη γραμμένο.


# Άσκηση 3:
    Στην άσκηση αυτή υλοποιήθηκε η Priority Queue, με την χρήση
    ADT Set. Ουσιαστικά, το priority_queue είναι ένα set
    και στις υλοποιήσεις των συναρτήσεων του, χρησιμοποιούνται
    (κυρίως) οι συναρτήσεις του Set. Άρα, η πολυπλότητα της
    κάθε μιας είναι αντίστοιχη με αυτή του Set.
    Το priority_queue_node είναι ένα set_node (με typedef)
    και χρησιμοποιούνται οι αντίστοιχες συναρτήσεις (κάνοντας
    cast κάθε φορά).
    Τα tests περνάνε χωρίς memory leaks (χωρίς να υπολογίζω
    την pqueue_update_order).


# Άσκηση 4:
    Στην άσκηση αυτή ζητήθηκε να υλοποιήσουμε την έννοια του
    κόμβου σε ένα PriorityQueue και κάποιες συμπληρωματικές
    λειτουργίες, οι οποίες αναφέρονται και στην εκφώνηση.

    Κάθε PriorityQueueNode περιέχει ένα Pointer για το value
    και ένα int για την θέση του μέσα στον σωρό.

    Οι αρχικές λειτουργίες του module, τροποποιήθηκαν ώστε να
    δέχονται κόμβους στο vector αντί για values μόνο. Οπότε
    όλες αυτές οι λειτουργίες παρέμειναν με την ίδια πολυπλοκότητα
    εκτός της destroy, στην οποία πρέπει να γίνει και ένα loop
    για την καταστροφή των κόμβων και των περιεχομένων τους. Άρα,
    έχουμε O(n).
    Ακόμα, υλοποιήθηκε η efficient_heapify, η οποία είναι ίδιας
    λογικής με αυτήν του εργαστηρίου, απλά αντί για value μόνο,
    στο vector της pqueue, αποθηκεύεται ένας κόμβος. Άρα, υπάρχει
    ένα loop, που κάνει n βήματα για να δημιουργήσει αυτούς τους
    κόμβους. Οπότε, όπως και στο εργαστήριο, έχουμε O(n) για να
    κάνουμε προσπέλαση όλα τα στοιχεία του αρχικού vector, αλλά
    τα στοιχεία τοποθετούνται στην σωστή θέση με bubble_down, η
    οποία είναι O(logn) και όπως είδαμε και στη διάλεξη, πάιρνουμε
    O(n), worst-case πολυπλοκότητα.

    Οι νέες υλοποιήσεις έχουν αντίστοιχα πολυπλοκότητες:
    
    - O(1), η pqueue_node_value, αφού εκτελείται 1 βήμα
    - O(logn), σαν worst-case, η pqueue_remove_node, αφού στην
        χειρότερη περίπτωση η bubble_down θα εκτελεστεί τόσες φορές
        όσες και το ύψος του δέντρου
    - O(logn), σαν worst-case, η pqueue_update_order, αφού στην
        χειρότερη περίπτωση θα εκτελεστεί ή η bubble_down, αν κάποιο
        παιδί έχει μεγαλύτερο value από το node, ή η bubble_up, αν
        ο γονιός έχει μικρότερο value από το node, ή καμία από τις δύο,
        αν το νέο value δεν επηρεάζει την διάταξη. Άμα εκτελεστεί κάποια
        από τις δύο, θα κάνει το πολύ logn βήματα, όσα δηλαδή το 
        ύψος του δέντρου


# Άσκηση 5:
    Στην άσκηση αυτή υλοποιήθηκαν οι λειτουργίες διάσχισης σε O(1),
    με την χρήση του ADTBList, το οποίο έχει υλοποιηθεί στην άσκηση 2.

    Στο struct set προστέθηκε κι ένα BList, σε κάθε SetNode προστέθηκε
    κι το αντίστοιχο BListNode (το οποίο σαν value έχει το SetNode).

    Ακόμα, κάθε φορά που καλείται το set_insert και το set_remove
    γίνεται insert/remove (αντίστοιχα) στο BList. Έτσι, διατηρείται
    ενημερωμένο το BList, κάθε φορά που προστίθεται ή αφαιρείται κάποιος
    κόμβος. Συνεπώς, οι λειτουργίες διάσχισης του Set (set_first, set_last,
    set_next, set_previous) μπορούν να υλοποιηθούν σε O(1), σταθερό χρόνο,
    αφού αρκεί να γίνει return το αντίστοιχο (first, last, next, previous)
    SetNode, με βάση τους κόμβους του BList, με τους οποίους υπάρχει εύκολη
    πρόσβαση στα παραπάνω.

    Οι πολυπλοκότητες στο set_insert και στο set_remove, παρέμειναν O(h),
    αφού για το SetNode, τόσο ήταν το complexity. Για την εισαγωγή/διαγραφή
    κάποιου BListNode, χρειάζεται να βρούμε τον κόμβο πάλι, οπότε έχουμε O(h).
    Άρα, όλη η πολυπλοκότητα του set_insert και ,αντίστοιχα, του set_remove
    είναι O(h).


# Bonus(5%) της Άσκησης 1:
    Υλοποιήθηκε και το bonus ερώτημα της 1ης Άσκησης. Έγινε ακριβώς
    η ίδια διαδικασία για το ADTPriorityQueue. Πάλι έγινε εισαγωγή
    του int steps στο struct priority_queue, ώστε κάθε φορά να μένει
    η τιμή από την τελευταία συνάρτηση που κλήθηκε. Το πρόγραμμα που βρίσκεται
    στο folder programs/pqueue_insert_steps εκτελεί την ίδια διαδικασία
    με το vector_insert_steps και βγάζει τα απότελεσματα για τον
    αντίστοιχο τρόπο insert. Το Makefile περιέχει τις γνωστές απλές
    λειτουργίες. Επίσης, σαν ARGS πρέπει να δοθεί είτε "real"
    είτε "amortized".

    Οι γραφικές παραστάσεις βρίσκονται στον folder
    images και παρατίθενται στο τέλος του README.

# Bonus(1η κουκίδα) της Άσκησης 5:
    Υλοποιήθηκε και δεύτερος τρόπος (με την δημιουργία έξτρα κόμβων στο
    κάθε SetNode), εκτός από την κλασσική υλοποίηση με BList. Οι λειτουργίες
    set_first, set_last, set_previous και set_next έχουν πολυπλοκότητα
    O(1). Η πολυπλοκότητα για το set_insert, set_remove είναι ίδια με πριν,
    δηλαδή O(h). Ακολουθείται η ίδια λογική που ακολουθήθηκε και στην βασική
    υλοποίηση, για τις 2 αυτές λειτουργίες (insert/remove).


# Γραφικές παραστάσεις (Άσκηση_1 και Bonus):
    
![vector_steps_real](https://github.com/chatziko-k08/2020-project-2-tech-gian/blob/master/images/vector_steps_real.JPG)
   
![vector_steps_amortized](https://github.com/chatziko-k08/2020-project-2-tech-gian/blob/master/images/vector_steps_amortized.JPG)

![pqueue_steps_real](https://github.com/chatziko-k08/2020-project-2-tech-gian/blob/master/images/pqueue_steps_real.JPG)
    
![pqueue_steps_amortized](https://github.com/chatziko-k08/2020-project-2-tech-gian/blob/master/images/pqueue_steps_amortized.JPG)
