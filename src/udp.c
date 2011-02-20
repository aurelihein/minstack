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
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <error.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>

#include "minstack_debug.h"
#include "private.h"
#include "udp.h"

/**
 * \brief Generate a minstack_udp
 * \param nickname is the name given to the minstack_udp could be NULL
 * \return the minstack_udp generated
 */
minstack_udp * minstack_udp_init(const char *nickname) {
    minstack_udp *mu = (minstack_udp *) calloc(1, sizeof(minstack_udp));
    if (mu == NULL) {
        printerror("Could not allocate a minstack_udp\n");
        return NULL;
    }
    mu->name = nickname;
    mu->type = NONE;

    mu->status = IDLE;
    mu->pthread_reading_thread_stop = 1;
    printmessage("%s minstack UDP has been initialized\n",mu->name);
    return mu;
}

/**
 * \brief Uninitialized a minstack_udp
 * \param mu is the minstack_udp stack
 */
void minstack_udp_uninit(minstack_udp *mu) {
    if (!mu) {
        printwarning("Trying to uninit a NULL minstack_udp\n");
        return;
    }
    if (mu->status == STARTED) {
        minstack_udp_stop(mu);
        printdebug("%s minstack UDP has been stopped before uninit\n",mu->name);
    }
    printmessage("%s minstack UDP has been uninitialized\n",mu->name);
    free(mu);
}

/**
 * \brief Stop the minstack_udp in parameter
 * \param mu the minstack_udp that have to be stopped
 * \return 0 if the minstack_udp stack stopped correctly
 */
int minstack_udp_stop(minstack_udp *mu) {
    if (!mu || mu->status == IDLE) {
        printerror("Cannot stop while not started\n");
        return -1;
    }
    if (mu->pthread_accept_thread && !mu->pthread_accept_thread_stop) {
        printdebug("The accepting thread is asked to stop\n");
        mu->pthread_accept_thread_stop = 1;
        pthread_join(mu->pthread_accept_thread, NULL);
        printmessage("The accepting thread stopped\n");
    }
    if (mu->pthread_reading_thread && !mu->pthread_reading_thread) {
        printdebug("The reading thread is asked to stop\n");
        mu->pthread_reading_thread = 1;
        pthread_join(mu->pthread_reading_thread, NULL);
        printmessage("The reading thread stopped\n");
    }
    if (mu->listen_socket_fd)
        close(mu->listen_socket_fd);
    pthread_mutex_destroy(&mu->mutex);
    mu->status = IDLE;
    printmessage("%s is now stopped\n",mu->name);
    return 0;
}

/**
 * \brief Init a minstack_udp stack as a server
 * \param mu is the minstack_udp stack
 * \param port is the port where the client could try to connect
 * \param max_client_number is the maximum number of clients that could be connected
 * \return 0 if OK
 */
int minstack_udp_init_server(minstack_udp *mu, int port, unsigned int max_client_number) {
    if (!mu || mu->type != NONE)
        return -1;
    pthread_mutex_init(&mu->mutex, NULL);
    pthread_mutex_lock(&mu->mutex);
    mu->type = SERVER;
    mu->port = port;
    mu->address = "127.0.0.1";//not used actually in server mode
    mu->receive_loop_usleep = 100 * 1000;
    mu->max_client_nb = max_client_number;
    mu->new_connection_callback = minstack_default_new_connection_callback;
    mu->connection_closed_callback = minstack_default_connection_closed_server_callback;
    mu->read_socket = minstack_udp_default_read;
    mu->external_read_socket = NULL;
    pthread_mutex_unlock(&mu->mutex);
    return 0;
}

/**
 * \brief Init a minstack_udp stack as a client
 * \param mu is the minstack_udp stack
 * \param port is the port where the client has to connect
 * \param address is the IP address where the client has to be connected
 * \return 0 if OK
 */
int minstack_udp_init_client(minstack_udp *mu, int port, const char *address) {
    if (!mu || mu->type != NONE)
        return -1;
    mu->type = CLIENT;
    mu->port = port;
    mu->address = address;
    mu->receive_loop_usleep = 100 * 1000;
    mu->new_connection_callback = NULL;
    mu->connection_closed_callback = NULL;
    mu->read_socket = minstack_udp_default_read;
    mu->external_read_socket = NULL;
    pthread_mutex_init(&mu->mutex, NULL);
    return 0;
}
