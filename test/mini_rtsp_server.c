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
#include <string.h>

enum all_status_rtsp_server{ IDLE=0, STREAMING};

#define RTSP_PORT   (554)

int run = 1;
int listenning_port = RTSP_PORT;
int handle_camera = 0;
enum all_status_rtsp_server rtsp_server_status;

void usage(const char *appli);
void stop(int exit_status);
void rtsp_listenner(int cid,char *buffer,unsigned int buffer_size_returned);
void send_status(int fd,int status_code, const char *version, int cseq_read);

int main (int argc, char **argv){
	minstack_tcp *mt_rtsp_server;

	signal(SIGABRT, stop);
	signal(SIGTERM, stop);
	signal(SIGINT, stop);

	if(argc != 2)
	    usage(argv[0]);

	handle_camera = (atoi(argv[1]))?1:0;

	printf("starting %s on the port %d %s\n",argv[0],listenning_port,(handle_camera)?"with a camera":"without camera");
	minstack_set_debug_level(MINSTACK_WARNING_LEVEL);
	mt_rtsp_server = minstack_tcp_start_a_server_with_read_function("The minstack server test", listenning_port, 10,rtsp_listenner);
	if(mt_rtsp_server == NULL)
	{
		printf("We could not initialized the server test...\n");
		return 0;
	}
	while(run)
	{
	    usleep(1*1000*1000);
	}
	printf("stopping %s\n",argv[0]);
	minstack_tcp_uninit(mt_rtsp_server);
	return 0;
}

void usage(const char *appli)
{
	printf("%s have to be started with the port number to listen too\n",appli);
	printf("example: %s 1    will start a RTSP server with a camera handled\n",appli);
	exit(0);
}

void stop(int exit_status)
{
	run = 0;
}

void rtsp_listenner(int cid,char *buffer,unsigned int buffer_size_returned)
{
    char method[14];
    char version[9];
    char link_used[256];
    int cseq_read = 0;
    int retval;
    char *p_buffer;

    if(!buffer || !buffer_size_returned)
        return ;

    retval = sscanf(buffer,"%s %s %s\r\nCSeq: %d",method,link_used,version,&cseq_read);
    if(retval!=4)
    {
        printf("No Cseq in the message\n");
        printf("#########################################################\n");
        printf("%s\n",buffer);
        printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        return;
    }

    if(!strcmp("OPTIONS",method))
    {
        char answer[1024];
        retval = snprintf(answer,sizeof(answer),"%s 200 OK\r\nCSeq: %d\r\nPublic: DESCRIBE, PLAY, TEARDOWN\r\n\r\n",version,cseq_read);
        write(cid,answer,retval);
    }
    else if(!strcmp("DESCRIBE",method))
    {
        char answer[1024];
        retval = snprintf(answer,sizeof(answer),"%s 200 OK\r\nCSeq: %d\r\nCamera: %d\r\nCodecs: %s\r\n\r\n",version,cseq_read,handle_camera,"H264");
        write(cid,answer,retval);
    }
    else if(!strcmp("PLAY",method))
    {
        char answer[1024];
        char rtsp_ip[24];//XXX.XXX.XXX.XXX.XXX.XXX
        int rtsp_port;
        char rtsp_codec[10];

        if(rtsp_server_status!=IDLE)
        {
            send_status(cid,455,version,cseq_read);//Method Not Valid in This State
            return;
        }
        p_buffer = strstr(buffer,"Ip:");
        if(!p_buffer)
        {
            send_status(cid,451,version,cseq_read);//Parameter Not Understood
            return;
        }
        sscanf(p_buffer,"Ip: %s\r\n",rtsp_ip);
        p_buffer = strstr(buffer,"Port:");
        if(!p_buffer)
        {
            send_status(cid,451,version,cseq_read);//Parameter Not Understood
            return;
        }
        sscanf(p_buffer,"Port: %d\r\n",&rtsp_port);
        p_buffer = strstr(buffer,"Codec:");
        if(!p_buffer)
        {
            send_status(cid,451,version,cseq_read);//Parameter Not Understood
            return;
        }
        sscanf(p_buffer,"Codec: %s\r\n",rtsp_codec);
        printf("We are going to stream to %s:%d with the codec %s\n",rtsp_ip,rtsp_port,rtsp_codec);
        send_status(cid,200,version,cseq_read);
        rtsp_server_status = STREAMING;
        return;
    }
    else if(!strcmp("TEARDOWN",method))
    {
        if(rtsp_server_status==STREAMING)
        {
            send_status(cid,200,version,cseq_read);
            printf("We are not streaming anymore\n");
            rtsp_server_status = IDLE;
            return;
        }
        else
        {
            printf("We are asked to not stream anymore but we were not streaming\n");
            send_status(cid,455,version,cseq_read);//Method Not Valid in This State
            return;
        }
    }else
        send_status(cid,405,version,cseq_read);//Method Not Allowed
}

void send_status(int fd,int status_code, const char *version, int cseq_read)
{
    char answer[1024];
    int retval;

    retval = snprintf(answer,sizeof(answer),"%s %d %s\r\nCSeq: %d\r\n\r\n",version, status_code,(status_code==200)?"OK":"KO",cseq_read);
    write(fd,answer,retval);
}
