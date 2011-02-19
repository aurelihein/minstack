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

#ifndef TCP_H_
#define TCP_H_

#ifdef __cplusplus
extern "C" {
#endif

struct minstack_tcp_struct;
/**
 * minstack_tcp is the main structure for TCP transactions
 * @ingroup TCP
 */
typedef struct minstack_tcp_struct minstack_tcp;

minstack_tcp * minstack_tcp_init(const char *nickname);
void minstack_tcp_uninit(minstack_tcp *mt);
int minstack_tcp_start(minstack_tcp *ts);
int minstack_tcp_stop(minstack_tcp *ts);
int minstack_tcp_init_server(minstack_tcp *ts, int port,unsigned int max_client_number);
int minstack_tcp_init_client(minstack_tcp *ts, int port, const char *address);
minstack_tcp *minstack_tcp_start_a_client(const char *nickname, int port, const char *address);
minstack_tcp *minstack_tcp_start_a_server(const char *nickname, int port, int max_client_number);
void minstack_tcp_set_receive_loop_usleep(minstack_tcp *ts,unsigned int usleepvalue);
unsigned int minstack_tcp_get_receive_loop_usleep(minstack_tcp *ts);
int minstack_tcp_write_to_client(minstack_tcp *ts,int cid,char *message,int len_message);
int minstack_tcp_write_to_server(minstack_tcp *mt,char *message,int len_message);
void minstack_tcp_printf(minstack_tcp *mt,const char *msg, ...);
int minstack_tcp_set_external_read_function(minstack_tcp *mt,void (*function)(int cid,char *buffer,unsigned int buffer_size_returned));
minstack_tcp *minstack_tcp_start_a_server_with_read_function(const char *nickname, int port, int max_client_number,void (*function)(int cid,char *buffer,unsigned int buffer_size_returned));

#ifdef __cplusplus
}
#endif

#endif /* TCP_H_ */
