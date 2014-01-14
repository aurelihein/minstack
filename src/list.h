/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                              minstack                                       *
 * Copyright (C) 2010,2011  Aurelien BOUIN (a_bouin@yahoo.fr)                  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  This program is free software; you can redistribute it and/or              *
 *  modify it under the terms of the GNU General Public License                *
 *  as published by the Free Software Foundation; either version 3             *
 *  of the License, or (at your option) any later version.                     *
 *                                                                             *
 *  This program is distributed in the hope that it will be useful,            *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 *  GNU General Public License for more details.                               *
 *                                                                             *
 *  You should have received a copy of the GNU General Public License          *
 *  along with this program; if not, write to the Free Software                *
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef LIST_H_
#define LIST_H_

#include "tools.h"

MinstackList * minstack_list_new(void *data);
MinstackList * minstack_list_append(MinstackList *elem, void * data);
MinstackList * minstack_list_prepend(MinstackList *elem, void *data);
MinstackList * minstack_list_concat(MinstackList *first, MinstackList *second);
MinstackList * minstack_list_free(MinstackList *list);
MinstackList * minstack_list_remove(MinstackList *first, void *data);
int minstack_list_size(const MinstackList *first);
void minstack_list_for_each(const MinstackList *list, void (*func)(void *));
void minstack_list_for_each2(const MinstackList *list, void (*func)(void *, void *), void *user_data);
MinstackList * minstack_list_remove_link(MinstackList *list, MinstackList *elem);
MinstackList * minstack_list_find(MinstackList *list, void *data);
MinstackList * minstack_list_find_custom(MinstackList *list, int (*compare_func)(const void *, const void*), const void *user_data);
void * minstack_list_nth_data(const MinstackList *list, int index);
int minstack_list_position(const MinstackList *list, MinstackList *elem);
int minstack_list_index(const MinstackList *list, void *data);
MinstackList * minstack_list_insert_sorted(MinstackList *list, void *data, int (*compare_func)(const void *, const void*));
MinstackList * minstack_list_insert(MinstackList *list, MinstackList *before, void *data);
MinstackList * minstack_list_copy(const MinstackList *list);

#define minstack_list_next(elem) ((elem)->next)

#endif
