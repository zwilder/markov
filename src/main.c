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

void print_help(void) {
    printf("Usage: markov [number] [input files]\n");
    printf("Where:\n\t[input files] are files containing words separated by a space.\n");
    printf("\t[number] is number of names to generate\n");
    printf("Example: \"markov 100 data1.txt data2.txt\" ");
    printf("will generate 100 random names\n using data1.txt and data2.txt as input.\n");
}

int main(int argc, char **argv) {
    init_genrand(time(NULL));
    HTable *ht = NULL;
    int i = 0;
    int n = 0;
    SList *words = NULL;
    SList *tmp = NULL;
    if(argc < 3) {
        print_help();
        return -1;
    }

    n = atoi(argv[1]);
    if(n < 1) {
        print_help();
        return -1;
    }
    words = slist_load_dataset(argv[2]);

    for(i = 3; i < argc; i++) {
        tmp = slist_load_dataset(argv[i]);
        if(tmp) {
            slist_add(&words,&tmp);
        } else {
            printf("Unable to load file: \"%s\"\n", argv[i]);
        }
    }
    if(words) {
        ht = markov_generate_ht(words);
        for(i = 0; i < n; i++) {
            generate_random_name(ht);
        }
        printf("\n");
        destroy_slist(&words);
        destroy_htable(ht);
    }

    return 0;
}
