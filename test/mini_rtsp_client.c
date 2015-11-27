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
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <tcp.h>
#include <minstack_debug.h>

#define USER_AGENT_TAG  ("mini_rtsp_client")
#define RTSP_OUR_PROTOCOL_TAG   ("RTSP/0.0")
#define RTSP_PORT   (554)

int run = 1;
int listenning_port = RTSP_PORT;

void usage(const char *appli);
void stop(int exit_status);
void listenner(int cid,char *from,int from_size, int *port, char *buffer,unsigned int buffer_size_returned);
void send_options(minstack_tcp *mt,const char *useragent,const char *ip);
void send_describe(minstack_tcp *mt,const char *useragent,const char *ip);
void send_play(minstack_tcp *mt,const char *useragent,const char *ip,const char *rtp_ip,int rtp_port, const char *rtp_codec);
void send_teardown(minstack_tcp *mt,const char *useragent,const char *ip);


int main (int argc, char **argv){
	minstack_tcp *mt_listen;
	if(argc != 5)
		usage(argv[0]);

	signal(SIGABRT, stop);
	signal(SIGTERM, stop);
	signal(SIGINT, stop);

	printf("Starting %s\n",argv[0]);
	printf("We are saying to %s to send videosupervision to %s:%d\n", argv[1],argv[2],atoi(argv[3]));
	mt_listen = minstack_tcp_init("The minstack client test");
	if(minstack_tcp_init_client(mt_listen,listenning_port,argv[1]))
	{
		printf("We could not initialized the client test...\n");
		return 0;
	}
	minstack_tcp_set_external_read_function(mt_listen,listenner);
	minstack_set_debug_level(MINSTACK_WARNING_LEVEL);
	minstack_tcp_start(mt_listen);
	usleep(100*1000);
    send_options(mt_listen,USER_AGENT_TAG,argv[1]);
    usleep(100*1000);
    send_describe(mt_listen,USER_AGENT_TAG,argv[1]);
    usleep(100*1000);
    if(!strcmp(argv[4],"play"))
    {
        send_play(mt_listen,USER_AGENT_TAG,argv[1],argv[2],atoi(argv[3]),"H264");
        printf("Should be playing Here !!!\n");
    }
    else
    {
        send_teardown(mt_listen,USER_AGENT_TAG,argv[1]);
        printf("Should be stopping Here !!!\n");
    }
    usleep(10*1000);
	printf("stopping %s\n",argv[0]);
	minstack_tcp_uninit(mt_listen);
	return 0;
}

void usage(const char *appli)
{
	printf("%s have to be started with the address of the XellIP and the IP and the port where the videosupervision is sent\n",appli);
    printf("example: %s \"192.168.49.251\" \"192.168.49.5\" 10000 play will ask 192.168.49.251 to send the video on 192.168.49.5:10000\n",appli);
    printf("example: %s \"192.168.49.251\" \"192.168.49.5\" 10000 stop will ask 192.168.49.251 to stop sending the video on 192.168.49.5:10000\n",appli);
	exit(0);
}

void stop(int exit_status)
{
	run = 0;
}

void listenner(int cid,char *from,int from_size, int *port, char *buffer,unsigned int buffer_size_returned)
{
    printf("##########################################################\n");
	printf("%s:%s",from,buffer);
	printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	//TO BE DONE
}

void send_options(minstack_tcp *mt,const char *useragent,const char *ip)
{
    printf("OPTIONS rtsp://%s %s\r\nCSeq: 1\r\nUser-Agent: %s\r\n\r\n",ip,RTSP_OUR_PROTOCOL_TAG,useragent);
    minstack_tcp_printf(mt,"OPTIONS rtsp://%s %s\r\nCSeq: 1\r\nUser-Agent: %s\r\n\r\n",ip,RTSP_OUR_PROTOCOL_TAG,useragent);
    minstack_tcp_printf(mt,"OPTIONS rtsp://%s %s\r\nCSeq: 1\r\nUser-Agent: TEST\r\n\r\n",ip,RTSP_OUR_PROTOCOL_TAG);
}

void send_describe(minstack_tcp *mt,const char *useragent,const char *ip)
{
    printf("DESCRIBE rtsp://%s %s\r\nCSeq: 2\r\nUser-Agent: %s\r\n\r\n",ip,RTSP_OUR_PROTOCOL_TAG,useragent);
    minstack_tcp_printf(mt,"DESCRIBE rtsp://%s %s\r\nCSeq: 2\r\nUser-Agent: %s\r\n\r\n",ip,RTSP_OUR_PROTOCOL_TAG,useragent);
}

void send_play(minstack_tcp *mt,const char *useragent,const char *ip,const char *rtp_ip,int rtp_port, const char *rtp_codec)
{
    printf("PLAY rtsp://%s %s\r\nCSeq: 3\r\nUser-Agent: %s\r\nIp: %s\r\nPort: %d\r\nCodec: %s\r\n\r\n",
            ip,RTSP_OUR_PROTOCOL_TAG,useragent,rtp_ip,rtp_port,rtp_codec);
    minstack_tcp_printf(mt,"PLAY rtsp://%s %s\r\nCSeq: 3\r\nUser-Agent: %s\r\nIp: %s\r\nPort: %d\r\nCodec: %s\r\n\r\n",
            ip,RTSP_OUR_PROTOCOL_TAG,useragent,rtp_ip,rtp_port,rtp_codec);
}

void send_teardown(minstack_tcp *mt,const char *useragent,const char *ip)
{
    printf("TEARDOWN rtsp://%s %s\r\nCSeq: 4\r\nUser-Agent: %s\r\n\r\n",ip,RTSP_OUR_PROTOCOL_TAG,useragent);
    minstack_tcp_printf(mt,"TEARDOWN rtsp://%s %s\r\nCSeq: 4\r\nUser-Agent: %s\r\n\r\n",ip,RTSP_OUR_PROTOCOL_TAG,useragent);
}
