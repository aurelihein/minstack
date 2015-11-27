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
void listenner(int cid,char *from,int from_size, int *port, char *buffer,unsigned int buffer_size_returned);
int mode = 0;
int count = 0;

#define MODE_1_SLEEP_TIME	(1000)

int main (int argc, char **argv){
	minstack_udp *mu_listen;
	int count = 1;
	if((argc < 3)||(argc > 4))
		usage(argv[0]);
	if(argc == 4){
		//using special test mode
		mode = atoi(argv[3]);
	}

	signal(SIGABRT, stop);
	signal(SIGTERM, stop);
	signal(SIGINT, stop);

	printf("starting %s on the address %s port %d\n",argv[0],argv[1],atoi(argv[2]));
	{
		switch(mode)
		{
		case 1 :
			while(run)
			{
				count++;
				mu_listen = minstack_udp_init("The minstack client test");
				if(minstack_udp_init_client(mu_listen,atoi(argv[2]),argv[1]))
				{
					printf("We could not initialized the client test...\n");
					return 0;
				}
				minstack_udp_set_external_read_function(mu_listen,listenner);
				minstack_set_debug_level(MINSTACK_WARNING_LEVEL);
				minstack_udp_start(mu_listen);
				minstack_udp_printf(mu_listen,"[1]STARTING:%d",count);
				usleep(MODE_1_SLEEP_TIME);
				minstack_udp_printf(mu_listen,"[0]WORKING_IDLE");
				usleep(MODE_1_SLEEP_TIME);
				minstack_udp_printf(mu_listen,"[0]WORKING_IN_COMMUNICATION");
				usleep(MODE_1_SLEEP_TIME);
				minstack_udp_printf(mu_listen,"[0]WORKING_IDLE");
				usleep(MODE_1_SLEEP_TIME);
				minstack_udp_printf(mu_listen,"[0]WORKING_IN_COMMUNICATION");
				usleep(MODE_1_SLEEP_TIME);
				minstack_udp_printf(mu_listen,"[1]STOPPED");
				usleep(MODE_1_SLEEP_TIME);
				minstack_udp_stop(mu_listen);
				minstack_udp_uninit(mu_listen);
				usleep(10*1000);
			}
			break;
		case 0 :
		default :
			mu_listen = minstack_udp_init("The minstack client test");
			if(minstack_udp_init_client(mu_listen,atoi(argv[2]),argv[1]))
			{
				printf("We could not initialized the client test...\n");
				return 0;
			}
			minstack_udp_set_external_read_function(mu_listen,listenner);
			minstack_set_debug_level(MINSTACK_WARNING_LEVEL);
			minstack_udp_start(mu_listen);
			while(run)
				minstack_udp_printf(mu_listen,"Hi server, It's %s, your best client that is talking to you %d!!!\n",argv[0],count++);
			printf("stopping %s\n",argv[0]);
			minstack_udp_stop(mu_listen);
			minstack_udp_uninit(mu_listen);
			break;
		}
	}
	return 0;
}

void usage(const char *appli)
{
	printf("%s have to be started with the address and the port number of the server, you can add the mode\n",appli);
	printf("%s [\"IP\"] [port] [option:mode]\n",appli);
	printf("mode 0 :standard\n");
	printf("mode 1 :simulation mediagip pooling\n");
	printf("example: %s \"127.0.0.1\" 10000   will start a client connected on the server 127.0.0.1:10000\n",appli);
	exit(0);
}

void stop(int exit_status)
{
	run = 0;
}

void listenner(int cid,char *from,int from_size, int *port, char *buffer,unsigned int buffer_size_returned)
{
	printf("<%s[%d]:%d>%s\n",from,*port,cid,buffer);
}

