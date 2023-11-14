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
/*****
 * Markov chain generator functions
 *****/
void slist_print(SList *head) {
    SList *tmp = head;
    int i = 1;
    while(tmp) {
        printf("%d - %s\n",i, tmp->data);
        tmp = tmp->next;
        i++;
    }
}

HTable* markov_generate_ht(char *fname) {
    /* Okay, so this needs to do the following:
     * - Open the dataset file
     * - Read 2 characters in the file (a1a2)
     * - Find every character that
     *   follows those two characters, add it to a CList
     * - Advance to next two characters (a2a3)
     * - Repeat until EOF
     *
     * Maybe this would be a good place to use SList, read the file and store
     * each name in a separate node?
     * - Open the dataset file
     * - Read and store strings in SList
     * - Loop through SList, doing:
     *   = take first two characters of string a1a2
     *   = add next character to CList a3
     *   = Look through rest of SList for any two characters that match
     *   = Add the following character to CList
     *   = Add to ht: key a1a2, CList
     *   = Repeat with next two characters in string a2a3
     *   = Continue until one of the characters is '\0'.
     *   = GO to next string in SList
     */

    HTable *ht = create_table(CAPACITY);
    FILE *f = fopen(fname, "r+");
    SList *words = NULL;
    SList *sit = NULL;
    char buf[100];
    char in = fgetc(f);
    int i = 0;
    char key[3];
    key[2] = '\0';
    buf[i] = '\0';

    // Read file, store words in SList
    while(in != EOF) {
        if((in == ' ') || (in == '\n')) {
            // End of word
            buf[i] = '\0';
            push_SList(&words, buf);
            i = 0;
        } else { 
            buf[i] = in;
            i++;
        }
        in = fgetc(f);
    }
    if((buf[i] != '\0') && (i > 0)) {
        // Don't miss the last word in the dataset
        buf[i] = '\0';
        push_SList(&words, buf);
    }

    // Loop through SList
    sit = words;
    i = 0;
    slist_print(words);
    while(sit) {
        if(strlen(sit->data) < 3) {
            sit = sit->next;
            continue;
        }
        for(i = 0; i < strlen(sit->data); i++)
        {
            key[0] = sit->data[i];
            key[1] = sit->data[i + 1];
            if(key[1] == '\0') break;
            ht_insert(ht, key, markov_find_match(key, words));
        }
        sit = sit->next;
    }

    // Cleanup
    destroy_SList(&words);
    fclose(f);
    return ht;
}

char markov_find_key_str(char *str, char *key) {
    char result = '\0';
    int i = 0;
    for (i = 0; i < (strlen(str)-2); i++) {
        if((str[i] == key[0]) && (str[i+1] == key[1])) {
            result = str[i+2];
            // Need to recursively call this function on the last part of the
            // string, str[i+3] to str[strlen(str)], to account for words that
            // have the key present more than once in it. TODO
            break;
        }
    }
    return result;
}

void string_to_lower(char *str) {
    int i = 0;
    for(i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

void slist_to_lower(SList *words) {
    SList *tmp = words;
    while(tmp) {
        string_to_lower(tmp->data);
        tmp = tmp->next;
    }
}

CList* markov_find_match(char *key, SList *words) {
    CList *result = NULL;
    SList *tmp = words;
    char c;
    // Look through SList for key, if found add the next character after the key
    // to the CList
    while(tmp) {
        //printf("Checking \"%s\" to see if it contains %s...\n", tmp->data,key);
        c = markov_find_key_str(tmp->data,key);
        if(c) {
            //printf("\tSuccess, %c follows %s!\n",c,key);
            if (result) {
                clist_push(&result,c);
            } else {
                result = create_clist_node(c);
            }
        }             
        tmp = tmp->next;
    }
    return result;
}

/*****
 * Random name functions
 *****/
