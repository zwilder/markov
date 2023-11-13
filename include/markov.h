
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
    CAPACITY = 50000 // Hash table size
};

/*****
 * Structure definitions
 *****/
typedef struct HTNode HTNode; // A node containing a string key and a list of chars
typedef struct HTable HTable; // The hash table
typedef struct HTList HTList; // List of HTNodes (used for Overflow buckets)
typedef struct CList CList; // List of characters

struct HTNode {
    char *key;
    CList *values;
};

struct HTable {
    HTNode **items;
    HTList **ofbuckets;
    int size;
    int count;
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

/*****
 * markov_gen.c
 *****/
// Markov chain generator functions
HTable* markov_generate_ht(char *fname);

// Random name functions

#endif //MARKOV_H
