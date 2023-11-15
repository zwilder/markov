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
HTable* markov_generate_ht(char *fname) {
    /* This function does the following:
     * - Open the dataset file (fname)
     * - Read and store strings in SList (words)
     * - Loop through SList (words), doing:
     *   = take first two characters of string (a1a2, "key")
     *   = call markov_find_match(key, words)
     *   markov_find_match(key, words) Does the following:
     *   -= Look through SList (words) for any two characters that match key
     *   -= Add the following character to CList
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
            slist_push(&words, buf);
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
        slist_push(&words, buf);
    }

    // Loop through SList
    sit = words;
    slist_to_lower(words);
    i = 0;
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
        // Add first two letters of word to starter key list
        key[0] = sit->data[0];
        key[1] = sit->data[1];
        if(!(ht->stkeys)) {
            ht->stkeys = create_slist(key);
        } else {
            slist_push(&(ht->stkeys),key);
        }
        sit = sit->next;
    }

    // Set maximum/minimum word length
    ht->wmax = slist_get_max(words);
    ht->wmin = slist_get_min(words);
    
    // Cleanup
    destroy_slist(&words);
    fclose(f);
    return ht;
}

char markov_find_key_str(char *str, char *key) {
    char result = '\0';
    int i = 0;
    for (i = 0; i < (strlen(str)-1); i++) {
        if((str[i] == key[0]) && (str[i+1] == key[1])) {
            result = str[i+2];
            if(!result) {
                //printf("Key \"%s\" found at end of \"%s\"\n",key,str);
                result = '!';
            }
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
            if(c == '!') c = '\0';
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
HTNode* ht_get_random_node(HTable *ht) {
    HTNode *result = NULL;
    /*
    int i = 0;
    while(!result) {
         i = mt_rand(0,ht->size);
        result = ht->items[i];
    }
    printf("HT Random node %d.\n",i);
    */
    int r = mt_rand(0,slist_count(ht->stkeys)-1);
    int i = 0;
    SList *key = ht->stkeys;
    for(i = 0; i < r; i++) {
        key = key->next;
    }
    if(key) {
        result = ht->items[ht_hash(key->data)];
    } else {
        printf("Item %d not found in hash table keys! i = %d.\n", r,i);
        slist_print(ht->keys);
    }

    return result;
}

char clist_get_random(CList *cl, int n) {
    char result = '\0';
    if(!n) return result;
    int i = mt_rand(0, n-1);
    int j = 0;
    CList *tmp = cl;
    for(j = 0; j < i; j++) {
        tmp = tmp->next;
    }
    if(tmp) {
        result = tmp->ch;
    }
    
    return result;
}

void generate_random_name(HTable *ht) {
    /* Need to:
     * - Choose random element from table to start name (key)
     * - Choose random following character from that value CList
     * - Look for next key made of key[1] and that character
     * - Continue until name is a max length or there is no values in CList
     *
     * What we need:
     * - List of not NULL values in HTable.
     */
    char name[100];
    char key[3];
    char c;
    int i = 0;
    int length = mt_rand(ht->wmin, ht->wmax);
    length = ht->wmax;
    HTNode *tmp = ht_get_random_node(ht);
    //strcpy(key,tmp->key);
    key[0] = tmp->key[0];
    key[1] = tmp->key[1];
    key[2] = '\0';
    //printf("Starting with %s.\n",key);
    
    name[0] = toupper(key[0]);
    name[1] = key[1];
    name[2] = '\0';
    while(strlen(name) <= ht->wmin) {
        for(i = 2; i < length; i++) {
            c = clist_get_random(tmp->values, tmp->nvalues);
            if(!c || !tmp) {
               name[i] = '\0';
               break; 
            }
            name[i] = c;
            key[0] = name[i-1];
            key[1] = name[i];
            //printf("->Key:%s\n",key);
            tmp = ht->items[ht_hash(key)];
        }
    }
    printf("%s\n",name);
    /*
    c = clist_get_random(tmp->values, tmp->nvalues);
    if(c) {
        name[2] = c;
        name[3] = '\0';
        printf("->Adding %c, %s\n",c,name);
    } else {
        name[2] = '\0';
        printf("->%s is end.\n",name);
    }
    */
}
