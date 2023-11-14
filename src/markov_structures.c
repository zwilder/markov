#include <markov.h>

/*****
 * Structure creation
 *****/

HTNode* create_htnode(char *key, CList *values) {
    HTNode *item = malloc(sizeof(HTNode));
    // Don't forget the \0 at the end of the string!
    item->key = malloc(sizeof(char) * (strlen(key) + 1));
    item->values = values;
    strcpy(item->key,key);
    return item;
}

HTable* create_table(int size) {
    int i = 0;
    HTable *table = malloc(sizeof(HTable));
    table->size = size;
    table->count = 0;
    // Calloc for clean fresh memory?
    table->items = calloc(table->size, sizeof(HTNode*));
    for(i = 0; i < table->size; i++) {
        table->items[i] = NULL;
    } 
    table->ofbuckets = create_ofbuckets(table);
    return table;
}

HTList** create_ofbuckets(HTable *table) {
    HTList **buckets = calloc(table->size, sizeof(HTList*));
    int i;
    for(i = 0; i < table->size; i++) {
        buckets[i] = NULL;
    }
    return buckets;
}

HTList* create_htlist(HTNode *item) {
    HTList *list = malloc(sizeof(HTList));
    if(!list) {
        return NULL;
    }
    list->data = item;
    list->next = NULL;
    return list;
}

CList* create_clist_node(char c) {
    CList *node = malloc(sizeof(CList));
    node->ch = c;
    node->next = NULL;
    return node;
}

/*****
 * Structure destruction
 *****/

void destroy_htnode(HTNode *item) {
    if(!item) {
        return;
    }
    free(item->key);
    destroy_clist(item->values);
    free(item);
}

void destroy_htable(HTable *table) {
    int i = 0;
    HTNode *item = NULL;
    for(i = 0; i < table->size; i++) {
        item = table->items[i];
        if(item) {
            destroy_htnode(item);
        }
    }
    destroy_ofbuckets(table);
    free(table->items);
    free(table);
}

void destroy_ofbuckets(HTable *table) {
    HTList **buckets = table->ofbuckets;
    int i;
    for(i = 0; i < table->size; i++) {
        destroy_htlist(buckets[i]);
    }
    free(buckets);
}

void destroy_htlist(HTList *headref) {
    HTList *tmp = headref;
    while(headref) {
        tmp = headref;
        headref = headref->next;
        free(tmp->data->key);
        destroy_clist(tmp->data->values);
        free(tmp->data);
        free(tmp);
    }
}

void destroy_clist_node(CList *node) {
    free(node);
}

void destroy_clist(CList *headref) {
    CList *tmp = headref;
    while(headref) {
        tmp = headref;
        headref = headref->next;
        destroy_clist_node(tmp);
    }
}

/*****
 * HTable functions
 *****/
unsigned long ht_hash(char *s) {
    // Hash from K&R, pg 144
    unsigned long hashval;

    for (hashval = 0; *s != '\0'; s++) {
        hashval = *s + 31 * hashval;
    }
    return hashval % CAPACITY;
    /* Note, must return an index within the capacity of the hash table,
     * otherwise it will access an unbounded memory location SEGFAULT */
}

void ht_collision(HTable *table, unsigned long index, HTNode *item) { 
    HTList *head = table->ofbuckets[index];
    if(!head) {
        /* Create the list */
        head = create_htlist(item);
        table->ofbuckets[index] = head;
    } else {
        table->ofbuckets[index] = htlist_insert(head, item);
    }
}

void ht_insert(HTable *table, char *key, CList *values) {
    HTNode *item = create_htnode(key,values);
    int index = ht_hash(key);
    HTNode *cur = table->items[index];
    CList *vlist = NULL, *ctmp = NULL;

    if(!cur) {
        /* Key does not exist */
        if(table->count == table->size) {
            /* Hash table full */
            printf("Insert error: Hash table full!\n");
            destroy_htnode(item);
            return;
        }
        table->items[index] = item;
        table->count += 1;
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
            destroy_htnode(item);
        } else {
            /* Different key, handle collision */
            ht_collision(table, index, item);
        }
    }
}

CList* ht_search(HTable *table, char *key) {
    int index = ht_hash(key);
    HTNode *item = table->items[index];
    HTList *head = table->ofbuckets[index];
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

void ht_delete(HTable *table, char *key) {
    int index = ht_hash(key);
    HTNode *item = table->items[index];
    HTList *head = table->ofbuckets[index];
    HTList *node = NULL;
    HTList *cur = NULL;
    HTList *prev = NULL;

    if(!item) {
        /*Item doesn't exist */
        return;
    }
    if(!head && (strcmp(item->key, key) == 0)) {
        /* No collision chain, remove item set table index to NULL */
        table->items[index] = NULL;
        destroy_htnode(item);
        table->count--;
        return;
    } else if (head) {
        /* Collision chain exists */
        if(strcmp(item->key, key) == 0) {
            /* Remove this item and set head of list as the new item */
            destroy_htnode(item);
            node = head;
            head = head->next;
            node->next = NULL;
            table->items[index] = create_htnode(node->data->key, 
                    node->data->values);
            destroy_htlist(node);
            table->ofbuckets[index] = head;
            return;
        }
        cur = head;
        prev = NULL;
        while(cur) {
            if(strcmp(cur->data->key, key) == 0) {
                if(!prev) {
                    /* First element of chain, remove chain */
                    destroy_htlist(head);
                    table->ofbuckets[index] = NULL;
                    return;
                } else {
                    /* Somewhere else in chain */
                    prev->next = cur->next;
                    cur->next = NULL;
                    destroy_htlist(cur);
                    table->ofbuckets[index] = head;
                    return;
                }
            }
            cur = cur->next;
            prev = cur;
        }
    }
}

void ht_print(HTable *table) {
    int i = 0;
    HTList *head = NULL;
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

void ht_print_item(HTable *table, char *key) {
    CList *val = ht_search(table, key);
    if(!val) {
        printf("Key: %s does not exist.\n", key);
    } else {
        printf("Key: %s | Value: ", key);
        clist_bracketprint(val);
    }
}
/*****
 * HTList functions
 *****/
HTList* htlist_insert(HTList *headref, HTNode *item) {
    HTList *newnode = create_htlist(item);
    HTList *tmp = NULL;
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

HTNode* htlist_pop(HTList **headref) {
    if(!(*headref)) {
        return NULL;
    }
    if(!(*headref)->next) {
        return NULL;
    }
    HTList *node = (*headref)->next;
    HTList *tmp = *headref;
    HTNode *item = NULL;
    tmp->next = NULL;
    *headref = node;
    memcpy(tmp->data, item, sizeof(HTNode));
    free(tmp->data->key);
    destroy_clist(tmp->data->values);
    free(tmp->data);
    free(tmp);
    return item;
}

/*****
 * CList functions
 *****/
void clist_push(CList **headref, char c) {
    CList *node = create_clist_node(c);
    node->next = (*headref);
    (*headref) = node;
}

void clist_print(CList *headref) {
    CList *tmp = headref;
    int i = 0;
    printf("CList contains the following:\n");
    while(tmp) {
        printf("\t%d) %c\n", i, tmp->ch);
        i++;
        tmp = tmp->next;
    }
}

void clist_bracketprint(CList *headref) {
    CList *tmp = headref;
    if(!tmp) {
        printf("\n");
        return;
    }
    printf("[");
    while(tmp) {
        printf("\'%c\'",tmp->ch);
        tmp = tmp->next;
        if(tmp) {
            printf(",");
        } else {
            printf("]\n");
        }
    }
}
