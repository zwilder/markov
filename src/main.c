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
    HTable *ht = NULL;
    int i = 0;
    SList *words = slist_load_dataset("data/german.txt");
    SList *a = slist_load_dataset("data/tolkeinesqe.txt");

    slist_add(&words, &a);
    
    ht = markov_generate_ht(words);
    ht_print(ht);
    printf("Longest word in dataset:%d. Shortest word in dataset: %d.\n",
            ht->wmax, ht->wmin);
    //printf("Keys:\n");
    //slist_print(ht->keys);
    //printf("Starter keys:\n");
    //slist_print(ht->stkeys);
    for(i = 0; i < 100; i++) {
        generate_random_name(ht);
    }
    destroy_slist(&words);
    destroy_htable(ht);

    return 0;
}
