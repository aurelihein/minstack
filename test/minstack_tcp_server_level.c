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
#include <tcp.h>
#include <minstack_debug.h>

int run = 1;

void usage(const char *appli);
void stop(int exit_status);
void listenner(int cid,char *from,int from_size, int *port, char *buffer,unsigned int buffer_size_returned);

int level = 0;

int main (int argc, char **argv){
	minstack_tcp *mt_listen;
	if(argc < 2 || argc > 3)
		usage(argv[0]);

	signal(SIGABRT, stop);
	signal(SIGTERM, stop);
	signal(SIGINT, stop);
    if(argc == 3)
        level = atoi(argv[2]);

	printf("starting %s on the port %d with level:%d\n",argv[0],atoi(argv[1]),level);
    minstack_set_debug_level(MINSTACK_WARNING_LEVEL);
	mt_listen = minstack_tcp_start_a_server_with_read_function("The minstack server level", atoi(argv[1]), 10,listenner);
	if(mt_listen == NULL)
	{
		printf("We could not initialized the server level...\n");
		return 0;
	}
	while(run)
		usleep(1*1000*1000);
	printf("stopping %s\n",argv[0]);
	minstack_tcp_uninit(mt_listen);
	return 0;
}

void usage(const char *appli)
{
	printf("%s have to be started with the port number to listen too\n",appli);
	printf("example: %s 10000 (debug_level)   will start a server that listen on the port 10000 with the debug level in option\n",appli);
	exit(0);
}

void stop(int exit_status)
{
	run = 0;
}

void listenner(int cid,char *from,int from_size, int *port, char *buffer,unsigned int buffer_size_returned)
{
    int level_read;
	if(buffer && buffer_size_returned)
	{
	    if(sscanf(buffer,"[%d]",&level_read) && (level_read >= level))
	        printf("%s[%d]:%s\n",from,*port,buffer);
	}
}

