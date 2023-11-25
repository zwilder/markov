/*
* Toolbox
* Copyright (C) Zach Wilder 2022-2023
* 
* This file is a part of Toolbox
*
* Toolbox is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* Toolbox is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with Toolbox.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <markov.h>

CList* create_clist_node(char c) {
    CList *node = malloc(sizeof(CList));
    node->ch = c;
    node->next = NULL;
    return node;
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

void clist_bracketwrite(CList *headref, FILE *f) {
    // Helper function for mht_write(...)
    CList *tmp = headref;
    if(!tmp) {
        fprintf(f,"\n");
        return;
    }
    fprintf(f,"[");
    while(tmp) {
        fprintf(f,"\'%c\'",tmp->ch);
        tmp = tmp->next;
        if(tmp) {
            fprintf(f,",");
        } else {
            fprintf(f,"]\n");
        }
    }
}

int clist_count(CList *cl) {
    if(!cl) return 0;
    return (clist_count(cl->next) + 1);
}
