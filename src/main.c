/*
* Markov Generator
* Copyright (C) Zach Wilder 2023
* 
* This file is a part of Markov Generator
*
* Markov Generator is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* Markov Generator is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with Markov Generator.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <markov.h>

int main(int argc, char **argv) {
    init_genrand(time(NULL));
    /* HTable test */
    /*
    CList *cl1, *cl2, *cl3, *cl4, *cl5;
    cl1 = create_clist_node('h');
    clist_push(&cl1,'k');
    cl2 = create_clist_node('c');
    clist_push(&cl2,'k');
    cl3 = create_clist_node('a');
    cl4 = create_clist_node('c');
    clist_push(&cl4,'l');
    clist_push(&cl4,'n');
    cl5 = create_clist_node(' ');

    HTable *ht = create_table(CAPACITY);
    ht_insert(ht, "ac", cl1);
    ht_insert(ht, "za", cl2);
    ht_insert(ht, "ch", cl3);
    ht_insert(ht, "ri", cl4);
    ht_insert(ht, "ch", cl5);
    ht_print(ht);

    printf("Deleting key \"ri\"...\n");
    ht_delete(ht, "ri");
    ht_print(ht);
    
    printf("Deleting key \"ac\"...\n");
    ht_delete(ht, "ac");
    ht_print(ht);
    
    destroy_htable(ht);
    */

    HTable *ht = markov_generate_ht("data/mnames_1000.txt");
    int i = 0;
    //ht_print(ht);
    //printf("Longest word in dataset:%d. Shortest word in dataset: %d.\n",
    //        ht->wmax, ht->wmin);
    //printf("Keys:\n");
    //slist_print(ht->keys);
    //printf("Starter keys:\n");
    //slist_print(ht->stkeys);
    for(i = 0; i < 100; i++) {
        generate_random_name(ht);
    }
    destroy_htable(ht);

    return 0;
}
