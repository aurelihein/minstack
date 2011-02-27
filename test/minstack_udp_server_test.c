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
#include <udp.h>
#include <minstack_debug.h>

int run = 1;

void usage(const char *appli);
void stop(int exit_status);
void listenner(int cid,char *buffer,unsigned int buffer_size_returned);

int main (int argc, char **argv){
	minstack_udp *mu_listen;
	if(argc != 2)
		usage(argv[0]);

	signal(SIGABRT, stop);
	signal(SIGTERM, stop);
	signal(SIGINT, stop);

	printf("starting %s on the port %d\n",argv[0],atoi(argv[1]));
	minstack_set_debug_level(MINSTACK_DEBUG_LEVEL);
#if 0
	mu_listen = minstack_udp_init("The minstack UDP server test");
	if(minstack_udp_init_server(mu_listen,atoi(argv[1]),10))
	{
		printf("We could not initialized the server test...\n");
		return 0;
	}
	minstack_udp_set_external_read_function(mu_listen,listenner);
	minstack_udp_start(mu_listen);
#else
	mu_listen = minstack_udp_start_a_server_with_read_function("The minstack server test", atoi(argv[1]), 10,listenner);
	if(mu_listen == NULL)
	{
		printf("We could not initialized the server test...\n");
		return 0;
	}
#endif
	while(run)
		usleep(1*1000*1000);
	printf("stopping %s\n",argv[0]);
#if 0
	minstack_udp_stop(mu_listen);
#endif
	minstack_udp_uninit(mu_listen);
	return 0;
}

void usage(const char *appli)
{
	printf("%s have to be started with the port number to listen too\n",appli);
	printf("example: %s 10000    will start a server that listen on the port 10000\n",appli);
	exit(0);
}

void stop(int exit_status)
{
	run = 0;
}

void listenner(int cid,char *buffer,unsigned int buffer_size_returned)
{
	if(buffer && buffer_size_returned)
	{
	    printf("<RECV>%s\n",buffer);
	}
}

