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

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>

#include "tools.h"
#include "list.h"
#include "minstack_debug.h"

MinstackList *minstack_list_new(void *data){
	MinstackList *new_elem=(MinstackList *)malloc(sizeof(MinstackList));
	new_elem->prev=new_elem->next=NULL;
	new_elem->data=data;
	return new_elem;
}

MinstackList * minstack_list_append(MinstackList *elem, void * data){
	MinstackList *new_elem=minstack_list_new(data);
	MinstackList *it=elem;
	if (elem==NULL) return new_elem;
	while (it->next!=NULL) it=minstack_list_next(it);
	it->next=new_elem;
	new_elem->prev=it;
	return elem;
}

MinstackList * minstack_list_prepend(MinstackList *elem, void *data){
	MinstackList *new_elem=minstack_list_new(data);
	if (elem!=NULL) {
		new_elem->next=elem;
		elem->prev=new_elem;
	}
	return new_elem;
}

MinstackList * minstack_list_concat(MinstackList *first, MinstackList *second){
	MinstackList *it=first;
	if (it==NULL) return second;
	while(it->next!=NULL) it=minstack_list_next(it);
	it->next=second;
	second->prev=it;
	return first;
}

MinstackList * minstack_list_free(MinstackList *list){
	MinstackList *elem = list;
	MinstackList *tmp;
	if (list==NULL) return NULL;
	while(elem->next!=NULL) {
		tmp = elem;
		elem = elem->next;
		free(tmp);
	}
	free(elem);
	return NULL;
}

MinstackList * minstack_list_remove(MinstackList *first, void *data){
	MinstackList *it;
	it=minstack_list_find(first,data);
	if (it) return minstack_list_remove_link(first,it);
	else {
		printwarning("minstack_list_remove: no element with %p data was in the list", data);
		return first;
	}
}int minstack_list_size(const MinstackList *first){
	int n=0;
	while(first!=NULL){
		++n;
		first=first->next;
	}
	return n;
}

void minstack_list_for_each(const MinstackList *list, void (*func)(void *)){
	for(;list!=NULL;list=list->next){
		func(list->data);
	}
}

void minstack_list_for_each2(const MinstackList *list, void (*func)(void *, void *), void *user_data){
	for(;list!=NULL;list=list->next){
		func(list->data,user_data);
	}
}

MinstackList *minstack_list_remove_link(MinstackList *list, MinstackList *elem){
	MinstackList *ret;
	if (elem==list){
		ret=elem->next;
		elem->prev=NULL;
		elem->next=NULL;
		if (ret!=NULL) ret->prev=NULL;
		free(elem);
		return ret;
	}
	elem->prev->next=elem->next;
	if (elem->next!=NULL) elem->next->prev=elem->prev;
	elem->next=NULL;
	elem->prev=NULL;
	free(elem);
	return list;
}

MinstackList *minstack_list_find(MinstackList *list, void *data){
	for(;list!=NULL;list=list->next){
		if (list->data==data) return list;
	}
	return NULL;
}

MinstackList *minstack_list_find_custom(MinstackList *list, int (*compare_func)(const void *, const void*), const void *user_data){
	for(;list!=NULL;list=list->next){
		if (compare_func(list->data,user_data)==0) return list;
	}
	return NULL;
}

void * minstack_list_nth_data(const MinstackList *list, int index){
	int i;
	for(i=0;list!=NULL;list=list->next,++i){
		if (i==index) return list->data;
	}
	printerror("minstack_list_nth_data: no such index in list.");
	return NULL;
}

void * minstack_list_nth2_data(const MinstackList *list, int index){
	int i;
	for(i=0;list!=NULL;list=list->next,++i){
		if (i==index) return list;
	}
	printerror("minstack_list_nth_data: no such index in list.");
	return NULL;
}

int minstack_list_position(const MinstackList *list, MinstackList *elem){
	int i;
	for(i=0;list!=NULL;list=list->next,++i){
		if (elem==list) return i;
	}
	printerror("minstack_list_position: no such element in list.");
	return -1;
}

int minstack_list_index(const MinstackList *list, void *data){
	int i;
	for(i=0;list!=NULL;list=list->next,++i){
		if (data==list->data) return i;
	}
	printerror("minstack_list_index: no such element in list.");
	return -1;
}

MinstackList *minstack_list_insert_sorted(MinstackList *list, void *data, int (*compare_func)(const void *, const void*)){
	MinstackList *it,*previt=NULL;
	MinstackList *nelem;
	MinstackList *ret=list;
	if (list==NULL) return minstack_list_append(list,data);
	else{
		nelem=minstack_list_new(data);
		for(it=list;it!=NULL;it=it->next){
			previt=it;
			if (compare_func(data,it->data)<=0){
				nelem->prev=it->prev;
				nelem->next=it;
				if (it->prev!=NULL)
					it->prev->next=nelem;
				else{
					ret=nelem;
				}
				it->prev=nelem;
				return ret;
			}
		}
		previt->next=nelem;
		nelem->prev=previt;
	}
	return ret;
}

MinstackList *minstack_list_insert(MinstackList *list, MinstackList *before, void *data){
	MinstackList *elem;
	if (list==NULL || before==NULL) return minstack_list_append(list,data);
	for(elem=list;elem!=NULL;elem=minstack_list_next(elem)){
		if (elem==before){
			if (elem->prev==NULL)
				return minstack_list_prepend(list,data);
			else{
				MinstackList *nelem=minstack_list_new(data);
				nelem->prev=elem->prev;
				nelem->next=elem;
				elem->prev->next=nelem;
				elem->prev=nelem;
			}
		}
	}
	return list;
}

MinstackList *minstack_list_copy(const MinstackList *list){
	MinstackList *copy=NULL;
	const MinstackList *iter;
	for(iter=list;iter!=NULL;iter=minstack_list_next(iter)){
		copy=minstack_list_append(copy,iter->data);
	}
	return copy;
}
