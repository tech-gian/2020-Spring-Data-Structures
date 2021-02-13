#include <stdio.h>
#include <stdlib.h>

#include "life.h"

#include "bmp.h"
#include "gif.h"


int main(int argc, char* argv[]) {
    // Παίρνουμε τα arguments
    // εκτός του zoom
    char* state_name = argv[1];

    int top = atoi(argv[2]);
    int left = atoi(argv[3]);
    int bottom = atoi(argv[4]);
    int right = atoi(argv[5]);

    int frames = atoi(argv[6]);

    int speed = atoi(argv[7]);
    int delay = atoi(argv[8]);

    char* gif_name = argv[9];


    // Φτιάχνουμε τα στοιχεία
    // για την δημιουργία του gif

    LifeState state = life_create_from_rle(state_name);

    ListNode loop;
    List list = life_evolve_many(state, frames, &loop);

    GIF* gif = gif_create(abs(right-left), abs(top-bottom));
    Bitmap* bitmap = bm_create(abs(right-left), abs(top-bottom));

    gif->default_delay = delay;

    // Μετατόπιση του i και του j
    // ώστε να ξεκινάει από το κέντρο του gif
    int i_dis = abs(top-bottom)/4;
    int j_dis = abs(right-left)/4;


    ListNode node = list_first(list);

    for (int k=0 ; k<frames ; k++) {
        // Για κάθε frame κάνουμε μαύρο το bitmap
        bm_set_color(bitmap, bm_atoi("black"));
        bm_clear(bitmap);
        state = list_node_value(list, node);
        
        // Κάνουμε speed εξελίξεις για κάθε frame
        for (int s=0 ; s<speed ; s++) {

            // Χρωματίζουμε το κάθε pixel ανάλογα
            // την τιμή value του κάθε κελιού
            for (int i=top-i_dis ; i<=bottom ; i++) {
                for (int j=left-j_dis ; j<=right ; j++) {

                    LifeCell cell = {j, i};
                    bool value = life_get_cell(state, cell);
                    int x = i+i_dis;
                    int y = j+j_dis;

                    if (value==true) {
                        bm_set_color(bitmap, bm_atoi("white"));
                        bm_fillrect(bitmap, x, y, x+1, y+1);
                    }
                    else {
                        bm_set_color(bitmap, bm_atoi("black"));
                        bm_fillrect(bitmap, x, y, x+1, y+1);
                    }
                }
            }

            // Ελέγχουμε αν υπάρχει loop ή οχι
            // και αντίστοιχα πάιρνουμε τον επόμενο κόμβο
            if (loop==NULL) {
                if (list_next(list, node)!=LIST_EOF) {
                    node = list_next(list, node);
                }
                else {
                    break;
                }
            }
            else {
                if (list_next(list, node)==LIST_EOF) {
                    node = loop;
                }
                else {
                    node = list_next(list, node);
                }
            }

        }

        gif_add_frame(gif, bitmap);

    }

    // Κάνουμε save το gif
    gif_save(gif, gif_name);

    // Κάνουμε free τα LifeState που υπάρχουν στην λίστα
    for (ListNode node=list_first(list);
        node!=LIST_EOF;
        node=list_next(list, node)) {

            LifeState to_free = list_node_value(list, node);
            life_destroy(to_free);
        }

    // Αποδεσμεύουμε την μνήμη
    bm_free(bitmap);
    gif_free(gif);

    // Αποδεσμεύουμε την λίστα (ό,τι έχει απομείνει)
    list_destroy(list);
}
