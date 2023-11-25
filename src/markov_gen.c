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
MHTable* markov_generate_mht(SList *words) {
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

    MHTable *ht = create_mhtable(CAPACITY);
    SList *sit = NULL;
    int i = 0;
    int j = 0;
    char key[KEYSZ+1];
    key[KEYSZ] = '\0';

    // Loop through SList
    sit = words;
    slist_to_lower(words);
    i = 0;
    j = 0;
    while(sit) {
        if(strlen(sit->data) < KEYSZ) {
            sit = sit->next;
            continue;
        }
        for(i = 0; i < strlen(sit->data); i++)
        {
            for(j = 0; j < KEYSZ; j++) {
                key[j] = sit->data[i+j];
            }
            if(key[KEYSZ-1] == '\0') break;
            mht_insert(ht, key, markov_find_match(key, words));
        }
        // Add first KEYSZ letters of word to starter key list
        for(j = 0; j < KEYSZ; j++) {
            key[j] = sit->data[j];
        }
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
    
    return ht;
}

char markov_find_key_str(char *str, char *key) {
    char result = '\0';
    int i = 0;
    int j = 0;
    char buf[KEYSZ+1];
    buf[KEYSZ] = '\0';
    for(i = 0; i < strlen(str)-KEYSZ+1; i++) {
        for(j = 0; j < KEYSZ; j++) {
            buf[j] = str[i+j];
        }
        if(strcmp(buf,key) == 0) {
            result = str[i+KEYSZ];
            if(!result) {
                result = '!';
            }
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

void string_to_upper(char *str) {
    int i = 0;
    for(i = 0; str[i]; i++) {
        str[i] = toupper(str[i]);
    }
}

void slist_to_lower(SList *words) {
    SList *tmp = words;
    while(tmp) {
        string_to_lower(tmp->data);
        tmp = tmp->next;
    }
}

void slist_to_upper(SList *words) {
    SList *tmp = words;
    while(tmp) {
        string_to_upper(tmp->data);
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
        c = markov_find_key_str(tmp->data,key);
        if(c) {
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
MHTNode* mht_get_random_node(MHTable *ht) {
    MHTNode *result = NULL;
    int r = mt_rand(0,slist_count(ht->stkeys)-1);
    int i = 0;
    SList *key = ht->stkeys;
    for(i = 0; i < r; i++) {
        key = key->next;
    }
    if(key) {
        result = mht_search_node(ht, key->data);
    } else {
        printf("Item %d not found in hash table keys! i = %d.\n", r,i);
        slist_print(ht->keys,'\n');
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

SList* generate_random_word(MHTable *ht,char *outf) {
    /* Need to:
     * - Choose random element from table to start name (key)
     * - Choose random following character from that value CList
     * - Look for next key made of key[1] and that character
     * - Continue until name is a max length or there is no values in CList
     */
    SList *result = NULL;
    char *name = malloc(sizeof(char) * 100);
    memset(name, '\0', 100);
    char key[KEYSZ + 1];
    char c;
    int i = 0;
    int j = 0;
    int k = 0;
    FILE *f = NULL;
    int length = mt_rand(ht->wmin, ht->wmax);
    length = ht->wmax;
    MHTNode *tmp = mht_get_random_node(ht);
    for(i = 0; i < KEYSZ; i++) {
        key[i] = tmp->key[i];
        name[i] = key[i];
    }
    key[KEYSZ] = '\0';
    name[KEYSZ] = '\0';
    name[0] = toupper(name[0]);
    
    for(i = KEYSZ; i < length; i++) {
        if(!tmp) {
            name[i] = '\0';
            break;
        }
        c = clist_get_random(tmp->values, tmp->nvalues);
        if(!c) {
           name[i] = '\0';
           break; 
        }
        name[i] = c;
        /* Leaving this here so future Zach won't look at that loop below like
         * it's some sort of dark black magic.
         * KEYSZ = 3
         * k = KEYSZ - 1 - j
         * key[0] = name[i-2] | j=0 k=2
         * key[1] = name[i-1] | j=1 k=1
         * key[2] = name[i]   | j=2 k=0
         */
        for(j = 0; j < KEYSZ; j++) {
            k = KEYSZ - 1 - j;
            key[j] = name[i-k];
        } 
        tmp = ht->items[mht_hash(key)];
    }
    if(outf) {
        f = fopen(outf, "a+");
        fprintf(f,"%s\n",name);
        fclose(f);
    } else {
        //printf("%s ",name);
        result = create_slist(name);
    }
    free(name);
    return(result);
}
