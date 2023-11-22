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
    int n = 10;
    int c = 0;
    SList *words = NULL;
    SList *tmp = NULL;
    char *outf = NULL;
    opterr = 0; // Don't show default errors
    while((c = getopt(argc,argv,"hsgn:o:")) != -1) {
        switch(c) {
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
        ht = markov_generate_ht(words);
        tmp = generate_random_word(ht,outf);
        for(i = 0; i < n; i++) {
            //slist_add(&tmp,&(generate_random_word(ht,outf)));
            slist_push_node(&tmp, generate_random_word(ht,outf));
        }
        if(!outf) slist_print(tmp, ' ');
        printf("\n");
        destroy_slist(&tmp);
        destroy_slist(&words);
        destroy_htable(ht);
    } 

    if(outf) {
        printf("%d words generated and written to %s\n", n, outf);
        free(outf);
    }
    
    return 0;
}

void print_help(void) {
    printf("Usage:\n\tmarkov [-n number] [-o outfile] infile1 [infile2...]\n");
    printf("\tmarkov -g infile1 -s infile2 [-n number] [-o outfile]\n");
    printf("Where:\n\tinfile1 [infile2...] are data files containing space separated words\n");
    printf("\t[-n number] is number of names to generate\n");
    printf("\t[-o outfile] is the file to write the output to\n");
    printf("\t-g infile1 -s infile2 are input data for a \"Genre species\" output\n");
    printf("Example: \"markov -n 100 data1.txt data2.txt\" ");
    printf("will generate 100 random names\n using data1.txt and data2.txt as input.\n");
    printf("Example: \"markov -n 100 -o out.txt -g data1.txt -s data2.txt\" ");
    printf("will generate 100 random \"Genre species\" style names, and write them to \"out.txt\"\n");
}
