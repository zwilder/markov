#include <markov.h>

int main(int argc, char **argv) {
    u_int8_t i = 32;
    CList *testo = create_clist_node(i);
    for(i = 33; i < 128; i++) {
        clist_push(&testo,i);
    }
    clist_print(testo);
    destroy_clist(testo);
    return 0;
}
