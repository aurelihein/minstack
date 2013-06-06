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
#ifdef WIN32 /* si vous tes sous Windows */
#include <winsock2.h>
#endif

#include "private.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

#ifdef WIN32
int win32_init_socket_api(void){
    WORD wVersionRequested;
    WSADATA wsaData;
    int i;

    wVersionRequested = MAKEWORD(2,2);
    if( (i = WSAStartup(wVersionRequested,  &wsaData))!=0)
    {
        fprintf(stderr,"Unable to initialize windows socket api, reason: %d (%s)",i,WSAGetLastError());
        return FALSE;
    }
    return TRUE;
}

int win32_uninit_socket_api(void){
    return WSACleanup();
}
#endif

int minstack_close(int fd){
#ifdef WIN32
    return closesocket(fd);
#else
    return close(fd);
#endif

}
