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
#ifndef SLIST_H
#define SLIST_H

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

struct SList {
    char *data;
    int length;
    struct SList *next;
};
typedef struct SList SList;

/*******************
 * slist.c functions
 *******************/
SList* create_SList(char *s);
SList* create_SList_blank(int strsize);
void push_SList_blank(SList **head, int strsize);
void destroy_SList(SList **head);
void push_SList(SList **head, char *s);
int count_SList(SList *node);
int count_chars_SList(SList *node, bool incSpace); 
char* get_string_SList(SList *node);
void add_SList(SList **to, SList **from); 
SList* split_string(char *s, char delim); 
SList* SList_linewrap(char *str, int w);

#endif
