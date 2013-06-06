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

#ifndef PRIVATE_H_
#define PRIVATE_H_

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>

#ifndef WIN32
#include <sys/socket.h>
#include <error.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif
#ifndef DEFAULT_READ_BUFFER_SIZE
#define DEFAULT_READ_BUFFER_SIZE	(256)
#endif


#define IPV4_LENGTH	(16)	//XXX.XXX.XXX.XXX0
#define MAX_CLIENT_CONNECTION	(5)

typedef struct {
	unsigned int connected_client_nb;
	int client_socket_fd[MAX_CLIENT_CONNECTION];
}sockets;

enum minstack_status_enum { IDLE=0, STARTED};

enum minstack_type_enum { NONE=0,SERVER,CLIENT};

//typedef int (*_common_vg_handle_message_fp)(common_connection_info*, global_structure*);
typedef int (*_minstack_tcp_new_connection)(int cid, struct sockaddr_in *cli_addr);
typedef int (*_minstack_tcp_connection_closed)(sockets *s, int cid);
typedef int (*_minstack_tcp_read_socket)(int cid,char *from, char **buffer);
typedef void (*_minstack_tcp_external_read_socket)(int cid, const char *from, char *buffer,unsigned int buffer_size_returned);
//typedef int (*_common_vg_get_local_mid_fp)(void);

typedef int (*_minstack_udp_read_socket)(int cid,char *from, char **buffer);
typedef void (*_minstack_udp_external_read_socket)(int cid, const char *from, char *buffer,unsigned int buffer_size_returned);

int minstack_close(int fd);

struct minstack_tcp_struct {
	const char *name;
	enum minstack_type_enum type;
	enum minstack_status_enum status;
	unsigned int port;
	const char *address;
	unsigned int receive_loop_usleep;
	int listen_socket_fd;
	unsigned int max_client_nb;
	pthread_mutex_t mutex;
	sockets sockets;
	_minstack_tcp_new_connection new_connection_callback;
	_minstack_tcp_connection_closed connection_closed_callback;
	_minstack_tcp_read_socket read_socket;
	_minstack_tcp_external_read_socket external_read_socket;
	pthread_t pthread_reading_thread;
	int pthread_reading_thread_stop;
	pthread_t pthread_accept_thread;
	int pthread_accept_thread_stop;
};

struct minstack_udp_struct {
    const char *name;
    enum minstack_type_enum type;
    enum minstack_status_enum status;
    unsigned int port;
    const char *address;
    unsigned int receive_loop_usleep;
    int listen_socket_fd;
    unsigned int max_client_nb;
    pthread_mutex_t mutex;
    _minstack_udp_read_socket read_socket;
    _minstack_udp_external_read_socket external_read_socket;
    pthread_t pthread_reading_thread;
    int pthread_reading_thread_stop;
};

void *get_in_addr(struct sockaddr *sa);

#ifdef WIN32
int win32_init_socket_api(void);
int win32_uninit_socket_api(void);
typedef int socklen_t;
#endif

#ifndef MSG_DONTWAIT
#define MSG_DONTWAIT    (0)
#endif

#endif /* PRIVATE_H_ */
