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

#include <mt19937.h>
#include <slist.h>

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
typedef struct HTNode HTNode; // A node containing a string key and a list of chars
typedef struct HTable HTable; // The hash table
typedef struct HTList HTList; // List of HTNodes (used for Overflow buckets)
typedef struct CList CList; // List of characters

struct HTNode {
    char *key;              // Key
    CList *values;          // List of characters
    int nvalues;            // Number of characters in CList
};

struct HTable {
    HTNode **items;         // Items in table
    HTList **ofbuckets;     // Overflow buckets
    int size;               // Hash table size
    int count;              // Items in hash table
    SList *keys;            // List of keys for random access
    int wmax;               // Longest word that should be generated
    int wmin;               // Shortest word that should be generated
    SList *stkeys;          // List of keys at the beginning of words
};

struct HTList {
    HTNode *data;
    HTList *next;
};

struct CList {
    char ch;
    CList *next;
};

/*****
 * markov_structures.c
 *****/
// Structure creation
HTNode* create_htnode(char *key, CList *values);
HTable* create_table(int size);
HTList** create_ofbuckets(HTable *table);
HTList* create_htlist(HTNode *item);
CList* create_clist_node(char c);

// Structure destruction
void destroy_htnode(HTNode *item);
void destroy_htable(HTable *table);
void destroy_ofbuckets(HTable *table);
void destroy_htlist(HTList *headref);
void destroy_clist_node(CList *node);
void destroy_clist(CList *headref);

// HTable functions
unsigned long ht_hash(char *str);
void ht_collision(HTable *table, unsigned long index, HTNode *item); 
void ht_insert(HTable *table, char *key, CList *values);
CList* ht_search(HTable *table, char *key);
void ht_delete(HTable *table, char *key);
void ht_print(HTable *table);
void ht_print_item(HTable *table, char *key);

// HTList functions
HTList* htlist_insert(HTList *headref, HTNode *item);
HTNode* htlist_pop(HTList **headref);

// CList functions
void clist_push(CList **headref, char c);
void clist_print(CList *headref);
void clist_bracketprint(CList *headref);
int clist_count(CList *cl);

/*****
 * markov_gen.c
 *****/
// Markov chain generator functions
HTable* markov_generate_ht(char *fname);
CList* markov_find_match(char *key, SList *words);
void string_to_lower(char *str);
void slist_to_lower(SList *words);
char markov_find_key_str(char *str, char *key);

// Random name functions
HTNode* ht_get_random_node(HTable *ht);
char clist_get_random(CList *cl, int n);
void generate_random_name(HTable *ht);

#endif //MARKOV_H
