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
#ifdef WIN32 /* Under Windows */
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include "private.h"
#include <sys/types.h>

#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 46
#endif

#ifdef WIN32
const char* win32_inet_ntop(int af, const void* src, char* dst, int cnt){

    struct sockaddr_in srcaddr;

    memset(&srcaddr, 0, sizeof(struct sockaddr_in));
    memcpy(&(srcaddr.sin_addr), src, sizeof(srcaddr.sin_addr));

    srcaddr.sin_family = af;
    if (WSAAddressToString((struct sockaddr*) &srcaddr, sizeof(struct sockaddr_in), 0, dst, (LPDWORD) &cnt) != 0) {
        DWORD rv = WSAGetLastError();
        printf("WSAAddressToString() : %d\n",rv);
        return NULL;
    }
    return dst;
}
#endif

// get const char * of sockaddr, IPv4 or IPv6:
const char *get_in_addr_char(struct sockaddr_storage *their_addr){
    char s[INET6_ADDRSTRLEN];
    void *in_addr;
    const char * returned="";
    struct sockaddr *sa = (struct sockaddr *)their_addr;

#ifndef WIN32
    in_addr = (sa->sa_family == AF_INET)?&(((struct sockaddr_in*)sa)->sin_addr):&(((struct sockaddr_in6*)sa)->sin6_addr);
	returned = inet_ntop(their_addr->ss_family, in_addr, s, sizeof s);
#else
    if(sa->sa_family == AF_INET){
    	in_addr = &(((struct sockaddr_in*)sa)->sin_addr);
	returned = win32_inet_ntop(their_addr->ss_family, in_addr, s, sizeof s);
    }else{
	//TO BE HANDLED
    	//in_addr = &(((struct sockaddr_in6*)sa)->sin6_addr);
	returned = "";
    }
#endif
	return returned;
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
