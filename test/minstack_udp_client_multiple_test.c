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
unsigned long loop = 0;
void usage(const char *appli);
void stop(int exit_status);

int main (int argc, char **argv){
	minstack_udp *mu_listen;
	if(argc != 3)
		usage(argv[0]);

	signal(SIGABRT, stop);
	signal(SIGTERM, stop);
	signal(SIGINT, stop);

	printf("starting %s on the address %s port %d\n",argv[0],argv[1],atoi(argv[2]));
	while(run)
	{
		loop++;
		printf("starting client %ld\n",loop);
		mu_listen = minstack_udp_start_a_client("multiple client test",atoi(argv[2]),argv[1]);
		if(!mu_listen)
		{
			printf("We could not initialized the client test...\n");
			return 0;
		}
		usleep(100*1000);
		minstack_udp_printf(mu_listen,"I am the %d\n",loop);
		minstack_udp_uninit(mu_listen);

	}
	printf("stopping %s\n",argv[0]);
	return 0;
}

void usage(const char *appli)
{
	printf("%s have to be started with the address and the port number of the server\n",appli);
	printf("example: %s \"127.0.0.1\" 10000   will start a client connected on the server 127.0.0.1:10000\n",appli);
	exit(0);
}

void stop(int exit_status)
{
	run = 0;
}
