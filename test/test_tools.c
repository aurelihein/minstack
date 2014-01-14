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
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "tcp.h"
#include "minstack_debug.h"
#include "tools.h"

int run = 1;

void usage(const char *appli);
void stop(int exit_status);

int main (int argc, char **argv){
	MinstackList *ml=NULL;
	/*
	if(argc != 4)
		usage(argv[0]);
	*/
	signal(SIGABRT, stop);
	signal(SIGTERM, stop);
	signal(SIGINT, stop);

	printf("Starting %s\n",argv[0]);

	minstack_add_message_into_last_position_of_list(&ml,"AUREL:hello world!!");
	minstack_add_message_into_last_position_of_list(&ml,"WORLD:Hello little guy!!");
	minstack_add_message_into_last_position_of_list(&ml,"AUREL:How are you ?");
	minstack_add_message_into_last_position_of_list(&ml,"WORLD:I am OK apart that human being are killing me !!");
	minstack_add_message_into_last_position_of_list(&ml,"AUREL:Are you talking about pollution ??");
	minstack_add_message_into_last_position_of_list(&ml,"WORLD:Yes I do ...!!");
	minstack_add_message_into_last_position_of_list(&ml,"AUREL:archhh ...");
	minstack_add_message_into_first_position_of_list(&ml,"Dialog Part I");

	printf("Message number:%d\n",minstack_get_number_of_messages_in_list(&ml));
	printf("Show all messages:\n");
	minstack_show_message_list(ml);

	char *ptr = minstack_pop_first_message_from_list(&ml);
	printf("The First message was %s\n",ptr);
	free(ptr);

	char *ptr2 = minstack_pop_last_message_from_list(&ml);
	printf("The Last message was %s\n",ptr2);
	free(ptr2);


	printf("Show all messages again:\n");
	minstack_show_message_list(ml);


	minstack_message_list_free(&ml);

	printf("Message number after free:%d\n",minstack_get_number_of_messages_in_list(&ml));

	printf("Ending %s\n",argv[0]);

	return 0;
}

void usage(const char *appli)
{
	printf("%s have to be started with the address and the port number of the server and the string to send\n",appli);
	printf("example: %s \"127.0.0.1\" 10000 \"status\"   will start a client connected on the server 127.0.0.1:10000 ans wend the string status\n",appli);
	exit(0);
}

void stop(int exit_status)
{
	run = 0;
}
