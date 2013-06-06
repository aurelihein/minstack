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

#ifndef UDP_H_
#define UDP_H_

#ifdef __cplusplus
extern "C" {
#endif

struct minstack_udp_struct;
/**
 * minstack_udp is the main structure for UDP transactions
 * @ingroup UDP
 */
typedef struct minstack_udp_struct minstack_udp;

minstack_udp * minstack_udp_init(const char *nickname);
int minstack_udp_init_client(minstack_udp *mu, int port, const char *address);
void minstack_udp_uninit(minstack_udp *mu);
minstack_udp *minstack_udp_start_a_client(const char *nickname, int port, const char *address);
minstack_udp *minstack_udp_start_a_server(const char *nickname, int port, int max_client_number);
int minstack_udp_init_server(minstack_udp *mu, int port,unsigned int max_client_number);
minstack_udp *minstack_udp_start_a_server_with_read_function(
        const char *nickname, int port, int max_client_number, void(*function)(
                int cid, const char *from,char *buffer, unsigned int buffer_size_returned));
int minstack_udp_set_external_read_function(minstack_udp *mu, void(*function)(int cid, const char *from,char *buffer, unsigned int buffer_size_returned)) ;
int minstack_udp_start(minstack_udp *mu);
int minstack_udp_stop(minstack_udp *mu);
int minstack_udp_write(minstack_udp *mu, char *message, int len_message);
void minstack_udp_printf(minstack_udp *mu, const char *msg, ...);
#define minstack_udp_printf_level0(fd , fmt, arg...)  minstack_udp_printf(fd,"[0]"fmt, ## arg)
#define minstack_udp_printf_level1(fd , fmt, arg...)  minstack_udp_printf(fd,"[1]"fmt, ## arg)
#define minstack_udp_printf_level2(fd , fmt, arg...)  minstack_udp_printf(fd,"[2]"fmt, ## arg)
#define minstack_udp_printf_level3(fd , fmt, arg...)  minstack_udp_printf(fd,"[3]"fmt, ## arg)
#define minstack_udp_printf_level4(fd , fmt, arg...)  minstack_udp_printf(fd,"[4]"fmt, ## arg)
#define minstack_udp_printf_level5(fd , fmt, arg...)  minstack_udp_printf(fd,"[5]"fmt, ## arg)
#ifdef __cplusplus
}
#endif

#endif /* UDP_H_ */
