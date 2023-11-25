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
    MHTable *ht = NULL;
    int i = 0;
    int n = 10;
    int c = 0;
    SList *words = NULL;
    SList *tmp = NULL;
    char *outf = NULL;
    bool log = false;
    FILE *f = NULL;
    opterr = 0; // Don't show default errors
    while((c = getopt(argc,argv,"flhsgn:o:")) != -1) {
        switch(c) {
            case 'l':
                log = true;
                break;
            case 'f':
            case 's':
            case 'g':
                i = generate_species(argc,argv);
                return i;
            case 'h':
                print_help();
                break;
            case 'n':
                n = atoi(optarg);
                if(n < 1) {
                    fprintf(stderr, "%d is less than 1.\n",n);
                    print_help();
                    return -1;
                }
                break;
            case 'o':
                //When we setup writing output to a file it goes here
                outf = strdup(optarg);
                break;
            case '?':
                if(optopt == 'n') {
                    fprintf(stderr, "Option -n requires a number.\n");
                } else if(optopt == 'o') {
                    fprintf(stderr, "Option -o requires a filename.\n");
                } else if(isprint(optopt)) {
                    fprintf(stderr, "Unkown option \'-%c\'\n", optopt);
                } else {
                    fprintf(stderr,
                          "Unkown option character \'\\x%x\'.\n",optopt);
                }
                print_help();
                return -1;
            default:
                break;
        }
    }
    for(i = optind; i < argc; i++) {
        if(!words) {
            words = slist_load_dataset(argv[i]);
            if(!words) {
                printf("Unable to load file: \"%s\"\n",argv[i]);
            }
        } else {
            tmp = slist_load_dataset(argv[i]);
            if(tmp) {
                slist_add(&words,&tmp);
            } else {
                printf("Unable to load file: \"%s\"\n",argv[i]);
            }
        }
    }

    if(words) {
        ht = markov_generate_mht(words);
        tmp = generate_random_word(ht,outf);
        for(i = 0; i < n; i++) {
            //slist_add(&tmp,&(generate_random_word(ht,outf)));
            slist_push_node(&tmp, generate_random_word(ht,outf));
        }
        if(!outf) slist_print(tmp, ' ');
        printf("\n");
        if(log) {
            f = fopen("log.txt","w+");
            log_separator(f);
            fprintf(f,"\nMarkov Word Generator Log File\n");
            log_separator(f);
            fprintf(f,"\nWords read from dataset:\n");
            for(i = optind; i < argc; i++) {
                fprintf(f,"\t%s\n",argv[i]);
            }
            fclose(f);
            slist_write(words, ' ', "log.txt", "a+");
            mht_write(ht, "log.txt", "a+");
        }
        destroy_slist(&tmp);
        destroy_slist(&words);
        destroy_mhtable(ht);
    } 

    if(outf) {
        printf("%d words generated and written to %s\n", n, outf);
        free(outf);
    }
    
    return 0;
}

