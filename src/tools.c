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

#include "list.h"
#include "tools.h"
#include "minstack_debug.h"

char **minstack_str_split_tools (char *s, const char *ct,int *nb_element)
{
   char **tab = NULL;

   if (s != NULL && ct != NULL && nb_element!= NULL)
   {
      int i;
      char *cs = NULL;
      size_t size = 1;

/* (1) */
      for (i = 0; (cs = strtok (s, ct)); i++)
      {
         if (size <= i + 1)
         {
            void *tmp = NULL;

/* (2) */
            size <<= 1;
            tmp = realloc (tab, sizeof (*tab) * size);
            if (tmp != NULL)
            {
               tab = tmp;
            }
            else
            {
               fprintf (stderr, "Memoire insuffisante\n");
               free (tab);
               tab = NULL;
               exit (EXIT_FAILURE);
            }
         }
/* (3) */
         tab[i] = cs;
         s = NULL;
      }
      tab[i] = NULL;
      *nb_element = i;
   }
   return tab;
}

int minstack_add_message_into_last_position_of_list(MinstackList **ml,const char *message){
	char *tmp = malloc(sizeof(char)*(strlen(message)+1));
	if(!tmp){
		printerror("Do not have enough space to add %s\n",message);
		return -1;
	}
	snprintf(tmp,strlen(message)+1,"%s",message);
	*ml = minstack_list_append(*ml,tmp);
	if(!*ml){
		printerror("Not able to get MinstackList");
		return -2;
	}
	return 0;
}

int minstack_add_message_into_first_position_of_list(MinstackList **ml,const char *message){
	char *tmp = malloc(sizeof(char)*(strlen(message)+1));
	if(!tmp){
		printerror("Do not have enough space to add %s\n",message);
		return -1;
	}
	snprintf(tmp,strlen(message)+1,"%s",message);
	*ml = minstack_list_prepend(*ml,tmp);
	if(!*ml){
		printerror("Not able to get MinstackList");
		return -2;
	}
	return 0;
}

//free the ouput
char *minstack_pop_last_message_from_list(MinstackList **ml){
	char *output=NULL;
	MinstackList *elem;

	elem = (MinstackList *)minstack_list_nth2_data(*ml,minstack_list_size(*ml)-1);
	output = (char *)elem->data;
	elem->data=NULL;
	elem=minstack_list_find(*ml,NULL);
	*ml = minstack_list_remove_link(*ml,elem);
	return output;
}

char *minstack_pop_first_message_from_list(MinstackList **ml){
	char *output=NULL;
	MinstackList *elem;

	elem = *ml;
	output = (char *)elem->data;
	elem->data=NULL;
	elem=minstack_list_find(*ml,NULL);
	*ml = minstack_list_remove_link(*ml,elem);

	return output;
}

void minstack_print_message(void *message){
	printf("%s\n",(char *)message);
}

int minstack_show_message_list(MinstackList *ml){
	if(ml == NULL)
		return -1;
	minstack_list_for_each(ml,(void (*)(void*))minstack_print_message);
	return 0;
}

int minstack_get_number_of_messages_in_list(MinstackList **ml){
	return minstack_list_size(*ml);
}

void minstack_message_list_free(MinstackList **ml){
	if(*ml)
		*ml=minstack_list_free(*ml);
}
