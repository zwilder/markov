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

#ifndef MARKOV_H
#define MARKOV_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

/*****
 * Toolbox
 *****/
#include <mt19937.h>
#include <slist.h>
#include <clist.h>

/*****
 * Demo program
 *****/
#include <markov_demo.h>

/*****
 * Constants
 *****/
enum {
    KEYSZ    = 3,    // Size of key used in chain
    CAPACITY = 10003 // Hash table size
};

/*****
 * Structure definitions
 *****/
typedef struct MHTNode MHTNode; // A node containing a string key and a list of chars
typedef struct MHTable MHTable; // The hash table
typedef struct MHTList MHTList; // List of MHTNodes (used for Overflow buckets)

struct MHTNode {
    char *key;              // Key
    CList *values;          // List of characters
    int nvalues;            // Number of characters in CList
};

struct MHTable {
    MHTNode **items;         // Items in table
    MHTList **ofbuckets;     // Overflow buckets
    int size;               // Hash table size
    int count;              // Items in hash table
    SList *keys;            // List of keys for random access
    int wmax;               // Longest word that should be generated
    int wmin;               // Shortest word that should be generated
    SList *stkeys;          // List of keys at the beginning of words
};

struct MHTList {
    MHTNode *data;
    MHTList *next;
};

/*****
 * markov_structures.c
 *****/
// Structure creation
MHTNode* create_mhtnode(char *key, CList *values);
MHTable* create_mhtable(int size);
MHTList** create_mht_ofbuckets(MHTable *table);
MHTList* create_mhtlist(MHTNode *item);

// Structure destruction
void destroy_mhtnode(MHTNode *item);
void destroy_mhtable(MHTable *table);
void destroy_mht_ofbuckets(MHTable *table);
void destroy_mhtlist(MHTList *headref);

// MHTable functions
unsigned long mht_hash(char *str);
void mht_collision(MHTable *table, unsigned long index, MHTNode *item); 
void mht_insert(MHTable *table, char *key, CList *values);
CList* mht_search(MHTable *table, char *key);
void mht_delete(MHTable *table, char *key);
void mht_print(MHTable *table);
void mht_write(MHTable *ht, char *fname, char *mode);
void mht_print_item(MHTable *table, char *key);

// MHTList functions
MHTList* mhtlist_insert(MHTList *headref, MHTNode *item);
MHTNode* mhtlist_pop(MHTList **headref);

/*****
 * markov_gen.c
 *****/
// Markov chain generator functions
MHTable* markov_generate_mht(SList *words);
CList* markov_find_match(char *key, SList *words);
void string_to_lower(char *str);
void slist_to_lower(SList *words);
void string_to_upper(char *str);
void slist_to_upper(SList *words);
char markov_find_key_str(char *str, char *key);

// Random name functions
MHTNode* mht_get_random_node(MHTable *ht);
char clist_get_random(CList *cl, int n);
SList* generate_random_word(MHTable *ht,char *outf);

#endif //MARKOV_H
