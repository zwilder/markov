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

void log_separator(FILE *f) {
    int i = 0;
    for(i=0; i < 20; i++) {
        fprintf(f,"*~");
    }
}

void print_help(void) {
    printf("Usage:\n\tmarkov [-l] [-n number] [-o outfile] infile1 [infile2...]\n");
    printf("\tmarkov -g infile1 -s infile2 [-l -f] [-n number] [-o outfile]\n");
    printf("Where:\n\tinfile1 [infile2...] are data files containing space separated words\n");
    printf("\t[-l] writes a log file to \"log.txt\" in the current directory\n");
    printf("\t[-n number] is number of names to generate\n");
    printf("\t[-o outfile] is the file to write the output to\n");
    printf("\t-g infile1 -s infile2 are input data for a \"Genre species\" output\n");
    printf("\t[-f] when used with -g -s, prints output as a \"First Last\" word.\n");
    printf("Example: \"markov -n 100 data1.txt data2.txt\" ");
    printf("will generate 100 random names\n using data1.txt and data2.txt as input.\n");
    printf("Example: \"markov -n 100 -o out.txt -g data1.txt -s data2.txt\" ");
    printf("will generate 100 random \"Genre species\" style names, and write them to \"out.txt\"\n");
}

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
    MHTable *ht = NULL;
    FILE *f = NULL;
    bool log = false;
    bool firstlast = false;
    char *gfile = NULL;
    char *sfile = NULL;
    opterr = 0; // Don't show default errors
    optind = 0; // Reset since we are reloading getopt
    while((c = getopt(argc,argv,"flhn:o:g:s:")) != -1) {
        switch(c) {
            case 'l':
                log = true;
                break;
            case 'g':
                genredat = slist_load_dataset(optarg);
                gfile = strdup(optarg);
                break;
            case 'f':
                firstlast = true;
                break;
            case 's':
                speciesdat = slist_load_dataset(optarg);
                sfile = strdup(optarg);
                break;
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
                if(gfile) free(gfile);
                if(sfile) free(sfile);
                return -1;
                break;
        }
    }
    if(!genredat || !speciesdat) {
        fprintf(stderr, "Missing genre or species file (-g [genrefile] -s [speciesfile])\n");
        print_help();
        destroy_slist(&genredat);
        destroy_slist(&speciesdat);
        if(outf) free(outf);
        if(gfile) free(gfile);
        if(sfile) free(sfile);
        return -1;
    }
    
    if(log) {
        f = fopen("log.txt","w+");
        log_separator(f);
        fprintf(f,"\nMarkov Word Generator Log File\n");
        fprintf(f,"\n\"Genre/Species\"\n");
        log_separator(f);
        fprintf(f,"\nWords read from datasets:\n");
        fprintf(f,"Genre: %s\n",gfile);
        fclose(f);
        slist_write(genredat, ' ', "log.txt","a+");
        f = fopen("log.txt","a+");
        log_separator(f);
        fprintf(f,"\nSpecies: %s\n",sfile);
        fclose(f);
        slist_write(speciesdat, ' ', "log.txt","a+");
        f = fopen("log.txt","a+");
        fprintf(f,"\n");
        log_separator(f);
        fclose(f);
    }

    //Generate genre
    ht = markov_generate_mht(genredat);
    genre = generate_random_word(ht, NULL);
    for(i = 0; i < n; i++) {
        //slist_add(&genre, &(generate_random_word(ht, NULL)));
        slist_push_node(&genre, generate_random_word(ht, NULL));
    }
    if (log) {
        f = fopen("log.txt","a+");
        fprintf(f,"\nHash table from %s:\n",gfile);
        fclose(f);
        mht_write(ht, "log.txt","a+");
    }
    destroy_mhtable(ht);

    //Generate species
    ht = markov_generate_mht(speciesdat);
    species = generate_random_word(ht, NULL);
    for(i = 0; i < n; i++) {
        //slist_add(&species, &generate_random_word(ht, NULL));
        slist_push_node(&species, generate_random_word(ht, NULL));
    }
    if (!firstlast) {
        slist_to_lower(species);
    }
    if (log) {
        f = fopen("log.txt","a+");
        fprintf(f,"Hash table from %s:\n",sfile);
        fclose(f);
        mht_write(ht, "log.txt","a+");
    }
    destroy_mhtable(ht);

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
    
    // Cleanup
    free(gfile);
    free(sfile);
    destroy_slist(&genredat);
    destroy_slist(&speciesdat);
    destroy_slist(&genre);
    destroy_slist(&species);
    return 0;
}
