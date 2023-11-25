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
 * Structure creation
 *****/

MHTNode* create_mhtnode(char *key, CList *values) {
    MHTNode *item = malloc(sizeof(MHTNode));
    // Don't forget the \0 at the end of the string!
    item->key = malloc(sizeof(char) * (strlen(key) + 1));
    item->values = values;
    strcpy(item->key,key);
    item->nvalues = clist_count(values);
    return item;
}

MHTable* create_mhtable(int size) {
    int i = 0;
    MHTable *table = malloc(sizeof(MHTable));
    table->size = size;
    table->count = 0;
    // Calloc for clean fresh memory?
    table->items = calloc(table->size, sizeof(MHTNode*));
    for(i = 0; i < table->size; i++) {
        table->items[i] = NULL;
    } 
    table->ofbuckets = create_mht_ofbuckets(table);
    table->keys = NULL;
    table->stkeys = NULL;
    table->wmax = 0;
    table->wmin = 0;
    return table;
}

MHTList** create_mht_ofbuckets(MHTable *table) {
    MHTList **buckets = calloc(table->size, sizeof(MHTList*));
    int i;
    for(i = 0; i < table->size; i++) {
        buckets[i] = NULL;
    }
    return buckets;
}

MHTList* create_mhtlist(MHTNode *item) {
    MHTList *list = malloc(sizeof(MHTList));
    if(!list) {
        return NULL;
    }
    list->data = item;
    list->next = NULL;
    return list;
}

/*****
 * Structure destruction
 *****/

void destroy_mhtnode(MHTNode *item) {
    if(!item) {
        return;
    }
    free(item->key);
    destroy_clist(item->values);
    free(item);
}

void destroy_mhtable(MHTable *table) {
    int i = 0;
    MHTNode *item = NULL;
    for(i = 0; i < table->size; i++) {
        item = table->items[i];
        if(item) {
            destroy_mhtnode(item);
        }
    }
    destroy_mht_ofbuckets(table);
    destroy_slist(&(table->keys));
    destroy_slist(&(table->stkeys));
    free(table->items);
    free(table);
}

void destroy_mht_ofbuckets(MHTable *table) {
    MHTList **buckets = table->ofbuckets;
    int i;
    for(i = 0; i < table->size; i++) {
        destroy_mhtlist(buckets[i]);
    }
    free(buckets);
}

void destroy_mhtlist(MHTList *headref) {
    MHTList *tmp = headref;
    while(headref) {
        tmp = headref;
        headref = headref->next;
        free(tmp->data->key);
        destroy_clist(tmp->data->values);
        free(tmp->data);
        free(tmp);
    }
}

/*****
 * MHTable functions
 *****/
unsigned long mht_hash(char *s) {
    // Hash from K&R, pg 144
    unsigned long hashval;

    for (hashval = 0; *s != '\0'; s++) {
        hashval = *s + 31 * hashval;
    }
    return hashval % CAPACITY;
    /* Note, must return an index within the capacity of the hash table,
     * otherwise it will access an unbounded memory location SEGFAULT */
}

void mht_collision(MHTable *table, unsigned long index, MHTNode *item) { 
    MHTList *head = table->ofbuckets[index];
    if(!head) {
        /* Create the list */
        head = create_mhtlist(item);
        table->ofbuckets[index] = head;
    } else {
        table->ofbuckets[index] = mhtlist_insert(head, item);
    }
}

void mht_insert(MHTable *table, char *key, CList *values) {
    MHTNode *item = create_mhtnode(key,values);
    int index = mht_hash(key);
    MHTNode *cur = table->items[index];
    CList *vlist = NULL, *ctmp = NULL;

    if(!cur) {
        /* Key does not exist */
        if(table->count == table->size) {
            /* Hash table full */
            printf("Insert error: Hash table full!\n");
            destroy_mhtnode(item);
            return;
        }
        table->items[index] = item;
        table->count += 1;
        // Update list of keys
        if(!(table->keys)) {
                table->keys = create_slist(key);
        } else {
            slist_push(&(table->keys), key); 
        }
    } else {
        /* Key exists at hash */
        if(strcmp(cur->key, key) == 0) {
            /* Same key, update value */
            vlist = cur->values;
            ctmp = values;
            while(ctmp) {
                clist_push(&vlist, ctmp->ch);
                ctmp = ctmp->next;
            }
            cur->values = vlist;
            destroy_mhtnode(item);
        } else {
            /* Different key, handle collision */
            mht_collision(table, index, item);
        }
    }
}

CList* mht_search(MHTable *table, char *key) {
    int index = mht_hash(key);
    MHTNode *item = table->items[index];
    MHTList *head = table->ofbuckets[index];
    while(item) {
        if(strcmp(item->key, key) == 0) {
            return item->values;
        }
        if(!head) {
            return NULL;
        }
        item = head->data;
        head = head->next;
    }
    return NULL;
}

void mht_delete(MHTable *table, char *key) {
    int index = mht_hash(key);
    MHTNode *item = table->items[index];
    MHTList *head = table->ofbuckets[index];
    MHTList *node = NULL;
    MHTList *cur = NULL;
    MHTList *prev = NULL;

    if(!item) {
        /*Item doesn't exist */
        return;
    }
    if(!head && (strcmp(item->key, key) == 0)) {
        /* No collision chain, remove item set table index to NULL */
        table->items[index] = NULL;
        destroy_mhtnode(item);
        slist_delete(&(table->keys),key); // Update key list
        table->count--;
        return;
    } else if (head) {
        /* Collision chain exists */
        if(strcmp(item->key, key) == 0) {
            /* Remove this item and set head of list as the new item */
            destroy_mhtnode(item);
            node = head;
            head = head->next;
            node->next = NULL;
            table->items[index] = create_mhtnode(node->data->key, 
                    node->data->values);
            destroy_mhtlist(node);
            table->ofbuckets[index] = head;
            return;
        }
        cur = head;
        prev = NULL;
        while(cur) {
            if(strcmp(cur->data->key, key) == 0) {
                if(!prev) {
                    /* First element of chain, remove chain */
                    destroy_mhtlist(head);
                    table->ofbuckets[index] = NULL;
                    return;
                } else {
                    /* Somewhere else in chain */
                    prev->next = cur->next;
                    cur->next = NULL;
                    destroy_mhtlist(cur);
                    table->ofbuckets[index] = head;
                    return;
                }
            }
            cur = cur->next;
            prev = cur;
        }
    }
}

void mht_print(MHTable *table) {
    int i = 0;
    MHTList *head = NULL;
    printf("\n\t********************\n");
    printf("\tHashTable\n\t********************\n");
    for(i = 0; i < table->size; i++) {
        if(table->items[i]) {
            printf("Index: %d | Key: %s | Values: ",
                    i, table->items[i]->key);
            clist_bracketprint(table->items[i]->values);
            if(table->ofbuckets[i]) {
                printf("\t=> Overflow Bucket =>\n");
                head = table->ofbuckets[i];
                while(head) {
                    printf("\tKey: %s | Values: ", head->data->key);
                    clist_bracketprint(head->data->values);
                    head = head->next;
                }
            }
        }
    }
    printf("\t********************\n\n");
}

void mht_print_item(MHTable *table, char *key) {
    CList *val = mht_search(table, key);
    if(!val) {
        printf("Key: %s does not exist.\n", key);
    } else {
        printf("Key: %s | Value: ", key);
        clist_bracketprint(val);
    }
}

void mht_write(MHTable *ht, char *fname, char *mode) {
    int i = 0;
    FILE *f = fopen(fname,mode);
    MHTList *head = NULL;
    fprintf(f,"\n\t********************\n");
    fprintf(f, "\tHashTable\n\t********************\n");
    for(i = 0; i < ht->size; i++) {
        if(ht->items[i]) {
            fprintf(f,"Index: %d | Key: %s | Values: ",
                    i, ht->items[i]->key);
            clist_bracketwrite(ht->items[i]->values,f);
            if(ht->ofbuckets[i]) {
                fprintf(f,"\t=> Overflow Bucket =>\n");
                head = ht->ofbuckets[i];
                while(head) {
                    fprintf(f,"\tKey: %s | Values: ", head->data->key);
                    clist_bracketwrite(head->data->values,f);
                    head = head->next;
                }
            }
        }
    }
    fprintf(f,"\t********************\n\n");
    fclose(f);
}
/*****
 * MHTList functions
 *****/
MHTList* mhtlist_insert(MHTList *headref, MHTNode *item) {
    MHTList *newnode = create_mhtlist(item);
    MHTList *tmp = NULL;
    if(!headref) {
        headref = newnode;
        return headref;
    }else if (!headref->next) {
        headref->next = newnode;
        return headref;
    }
    tmp = headref;
    while(tmp->next->next) {
        /* Gets last node */
        tmp = tmp->next;
    }
    tmp->next = newnode;
    return headref;
}

MHTNode* mhtlist_pop(MHTList **headref) {
    if(!(*headref)) {
        return NULL;
    }
    if(!(*headref)->next) {
        return NULL;
    }
    MHTList *node = (*headref)->next;
    MHTList *tmp = *headref;
    MHTNode *item = NULL;
    tmp->next = NULL;
    *headref = node;
    memcpy(tmp->data, item, sizeof(MHTNode));
    free(tmp->data->key);
    destroy_clist(tmp->data->values);
    free(tmp->data);
    free(tmp);
    return item;
}

