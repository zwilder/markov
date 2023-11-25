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

#ifndef CLIST_H
#define CLIST_H

typedef struct CList CList; // List of characters

struct CList {
    char ch;
    CList *next;
};

CList* create_clist_node(char c);
void destroy_clist_node(CList *node);
void destroy_clist(CList *headref);

// CList functions
void clist_push(CList **headref, char c);
void clist_print(CList *headref);
void clist_bracketprint(CList *headref);
void clist_bracketwrite(CList *headref, FILE *f);
int clist_count(CList *cl);

#endif //CLIST_H
