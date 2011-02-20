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
void minstack_udp_uninit(minstack_udp *mu);
int minstack_udp_init_server(minstack_udp *mu, int port,unsigned int max_client_number);

int minstack_udp_stop(minstack_udp *mu);

#ifdef __cplusplus
}
#endif

#endif /* UDP_H_ */
