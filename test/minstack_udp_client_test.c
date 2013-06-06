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
void listenner(int cid,const char *from,char *buffer,unsigned int buffer_size_returned);

int main (int argc, char **argv){
	minstack_udp *mu_listen;
	if(argc != 4)
		usage(argv[0]);

	signal(SIGABRT, stop);
	signal(SIGTERM, stop);
	signal(SIGINT, stop);

	printf("Starting %s on the address %s port %d sending %s\n",argv[0],argv[1],atoi(argv[2]),argv[3]);
	mu_listen = minstack_udp_init("The minstack client test");
	if(minstack_udp_init_client(mu_listen,atoi(argv[2]),argv[1]))
	{
		printf("We could not initialized the client test...\n");
		return 0;
	}
	minstack_udp_set_external_read_function(mu_listen,listenner);
	minstack_set_debug_level(MINSTACK_WARNING_LEVEL);
	minstack_udp_start(mu_listen);
	usleep(100*1000);
	minstack_udp_printf(mu_listen,"%s",argv[3]);
	printf("Stopping %s\n",argv[0]);
	minstack_udp_stop(mu_listen);
	minstack_udp_uninit(mu_listen);
	return 0;
}

void usage(const char *appli)
{
	printf("%s have to be started with the address and the port number of the server and after add the char sequence to send\n",appli);
	printf("%s [\"IP\"] [port] [\"char sequence\"]\n",appli);
	printf("example: %s \"127.0.0.1\" 10000   will start a client connected on the server 127.0.0.1:10000\n",appli);
	exit(0);
}

void stop(int exit_status)
{
	run = 0;
}

void listenner(int cid,const char *from, char *buffer,unsigned int buffer_size_returned)
{
	printf("<RECV>%s:%s\n",from,buffer);
}

