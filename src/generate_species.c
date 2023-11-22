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

int generate_species(int argc,char **argv) {
    int c = 0;
    int i = 0;
    int n = 10;
    SList *genredat = NULL;
    SList *speciesdat = NULL;
    SList *genre = NULL;
    SList *species = NULL;
    SList *itg = NULL;
    SList *its = NULL;
    char *outf = NULL;
    HTable *ht = NULL;
    FILE *f = NULL;
    opterr = 0; // Don't show default errors
    optind = 0; // Reset since we are reloading getopt
    while((c = getopt(argc,argv,"hn:o:g:s:")) != -1) {
        switch(c) {
            case 'g':
                genredat = slist_load_dataset(optarg);
                break;
            case 's':
                speciesdat = slist_load_dataset(optarg);
                break;
            case 'h':
                print_help();
                break;
            case 'n':
                n = atoi(optarg);
                if(n < 1) {
                    print_help();
                    return -1;
                }
                break;
            case 'o':
                outf = strdup(optarg);
                break;
            case '?':
                if(optopt == 'n') {
                    fprintf(stderr, "Option -n requires a number.\n");
                } else if(optopt == 'o' || optopt == 'g' || optopt == 's') {
                    fprintf(stderr, "Option -%c requires a filename.\n",optopt);
                } else if(isprint(optopt)) {
                    fprintf(stderr, "Unkown option \'-%c\'\n", optopt);
                } else {
                    fprintf(stderr,
                          "Unkown option character \'\\x%x\'.\n",optopt);
                }
                print_help();
                destroy_slist(&genredat);
                destroy_slist(&speciesdat);
                if(outf) free(outf);
                return -1;
                break;
        }
    }
    if(!genredat || !speciesdat) {
        print_help();
        destroy_slist(&genredat);
        destroy_slist(&speciesdat);
        if(outf) free(outf);
        return -1;
    }

    //Generate genre
    ht = markov_generate_ht(genredat);
    genre = generate_random_word(ht, NULL);
    for(i = 0; i < n; i++) {
        //slist_add(&genre, &(generate_random_word(ht, NULL)));
        slist_push_node(&genre, generate_random_word(ht, NULL));
    }
    destroy_htable(ht);

    //Generate species
    ht = markov_generate_ht(speciesdat);
    species = generate_random_word(ht, NULL);
    for(i = 0; i < n; i++) {
        //slist_add(&species, &generate_random_word(ht, NULL));
        slist_push_node(&species, generate_random_word(ht, NULL));
    }
    destroy_htable(ht);
    slist_to_lower(species);

    //Write outf
    if(outf) {
        f = fopen(outf, "w+");
        if(!f) {
            printf("Error writing outfile: %s\n",outf);
            return -1;
        }
        itg = genre;
        its = species;
        while(itg) {
            fprintf(f,"%s %s\n", itg->data, its->data);
            itg = itg->next;
            its = its->next;
        }
        fclose(f);
        printf("%d \"Genre species\" written to %s\n",n,outf);
        free(outf);
    } else {
        itg = genre;
        its = species;
        while(itg && its) {
            printf("%s %s\n", itg->data, its->data);
            itg = itg->next;
            its = its->next;
        }
    }
    destroy_slist(&genredat);
    destroy_slist(&speciesdat);
    destroy_slist(&genre);
    destroy_slist(&species);
    return 0;
}
