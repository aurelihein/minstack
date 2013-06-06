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
#include <sys/types.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>
#ifndef WIN32
#include <sys/socket.h>
#include <error.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#else
#include <winsock2.h>
#endif

#include "minstack_debug.h"
#include "private.h"
#include "tcp.h"

int stop_thread_server_socket = 0;

int minstack_tcp_delete_cid(sockets *mt, int cid);
int minstack_tcp_recvfrom_read(int cid, char *from, char **buffer);
char *minstack_tcp_default_read(int cid, unsigned int *buffer_size_returned);

int minstack_default_new_connection_callback(int cid,
        struct sockaddr_in *cli_addr) {
    printmessage("Accepted client: %s:%d socket[%d]\n",
            inet_ntoa(cli_addr->sin_addr),
            ntohs(cli_addr->sin_port),cid);
    return 0;
}

int minstack_default_connection_closed_server_callback(sockets *socket, int cid) {
    printmessage("The client closed the socket %d\n",cid);
    minstack_tcp_delete_cid(socket, cid);
    return 0;
}

int minstack_tcp_delete_cid(sockets *mt, int cid) {
    int i;
    for (i = 0; i < mt->connected_client_nb; i++) {
        printdebug("%s socket:%d\n",__FUNCTION__,cid);
        if (mt->client_socket_fd[i] == cid) {
            //we have found the cid
            for (; i < (mt->connected_client_nb - 1); i++) {
                mt->client_socket_fd[i] = mt->client_socket_fd[i + 1];
            }
            mt->connected_client_nb--;
            return 0;
        }
    }
    return -1;
}
#define FD_SETMAX(s, ss, m) { FD_SET((s), ss); if ((s) > m) m = (s); }

void *minstack_tcp_reading_thread(void *ptr) {
    minstack_tcp *mt = (minstack_tcp *) ptr;
    fd_set fds_read;
    int fdmax;
    struct timeval tv;

    printdebug("starting %s\n",__FUNCTION__);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    mt->pthread_reading_thread_stop = 0;
    while (!mt->pthread_reading_thread_stop) {
        int i, retval, socket_to_read = -1;
        fdmax = 0;
        tv.tv_sec = 0;
        tv.tv_usec = (mt->receive_loop_usleep);
        FD_ZERO(&fds_read);
        pthread_mutex_lock(&mt->mutex);
        if (mt->sockets.connected_client_nb == 0) {
            pthread_mutex_unlock(&mt->mutex);
            usleep(mt->receive_loop_usleep);
            //printdebug("there is no client\n");
#ifdef	STOP_READING_THREAD_WITHOUT_CLIENTS
            break;
#else
            continue;
#endif
        }
        for (i = 0; i < mt->sockets.connected_client_nb; i++) {
            printdebug("Stay socket %d\n",mt->sockets.client_socket_fd[i]);
            FD_SETMAX(mt->sockets.client_socket_fd[i],&fds_read,fdmax);
        }
        fdmax++;
        pthread_mutex_unlock(&mt->mutex);

        retval = select(fdmax, &fds_read, NULL, NULL, &tv);
        pthread_mutex_lock(&mt->mutex);
        if (retval == EBADF) {
            printmoreerror("Select Bad FD\n");
            continue;
        }
        if (retval < 0) {
            printerror("Select: got an error %d\n",retval);
            printmoreerror("Select:");
            break;
        }
        if (retval) {
            printdebug("there is something to read\n");
            for (i = 0; i < mt->sockets.connected_client_nb; i++) {
                if (FD_ISSET(mt->sockets.client_socket_fd[i],&fds_read)) {
                    socket_to_read = mt->sockets.client_socket_fd[i];
                }
            }
        }
        if (!retval) {
            //printmoreerror("Nothing much from the select\n");
        }
        pthread_mutex_unlock(&mt->mutex);
        if (socket_to_read > 0) {
        	char from[16];
            char *buffer=NULL;
            int buffer_size = 0;
            pthread_mutex_lock(&mt->mutex);
            pthread_mutex_unlock(&mt->mutex);
            buffer_size = mt->read_socket(socket_to_read, from,&buffer);

            if (buffer_size <= 0) {
                pthread_mutex_lock(&mt->mutex);
                if (mt && mt->connection_closed_callback
                        && (mt->type == SERVER)) {
                    mt->connection_closed_callback(&mt->sockets, socket_to_read);
                    minstack_close(socket_to_read);
                    printdebug("Closing the socket %d\n",socket_to_read);
                }
                pthread_mutex_unlock(&mt->mutex);
                if (mt && (mt->type == CLIENT)) {
                    printmessage("The server do not exists anymore we will close the connection\n");
                    minstack_close(socket_to_read);
                    minstack_tcp_stop(mt);
                }
            } else {
                printmessage("%s received from %s(%d):(%u)=>%s\n",mt->name,from, socket_to_read,buffer_size,buffer);
                if (mt->external_read_socket)
                    mt->external_read_socket(socket_to_read, from, buffer, buffer_size);
                //else
                free(buffer);
            }
        }

    }
    mt->pthread_reading_thread_stop = 1;
    printdebug("stopping %s\n",__FUNCTION__);
    pthread_exit(NULL);
    return NULL;
}
/*
 * take ressources because pthread_cancel do not free the memory allocation
 * but we have to cancel the thread because we can stay in accept wait event if we try to use a non blocked socket
 */
void *minstack_tcp_accept_thread(void *ptr) {
    minstack_tcp *mt = (minstack_tcp *) ptr;
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    printdebug("starting %s\n",__FUNCTION__);
    while (!mt->pthread_accept_thread_stop) {
        int tmp_clisockfd;
        unsigned int cli_len;
        struct sockaddr_in cli_addr;

        memset((char *) &cli_addr, 0, sizeof(cli_addr));
        cli_len = sizeof(cli_addr);
#ifdef WIN32
        tmp_clisockfd = accept(mt->listen_socket_fd, (struct sockaddr *) &cli_addr,(int *) &cli_len);
#else
        tmp_clisockfd = accept(mt->listen_socket_fd, (struct sockaddr *) &cli_addr, &cli_len);
#endif
        if (tmp_clisockfd == -1) {
            usleep(mt->receive_loop_usleep);
            continue;
        } else if (tmp_clisockfd <= 0) {
            printerror("error :%d\n",tmp_clisockfd);
            printmoreerror("ERROR on accept:");
            break;
        }
        pthread_mutex_lock(&mt->mutex);
        mt->sockets.client_socket_fd[mt->sockets.connected_client_nb]
                = tmp_clisockfd;
        mt->sockets.connected_client_nb++;
        pthread_mutex_unlock(&mt->mutex);
        mt->new_connection_callback(tmp_clisockfd, &cli_addr);
        if (mt->pthread_reading_thread_stop && pthread_create(
                &mt->pthread_reading_thread, NULL, minstack_tcp_reading_thread,
                (void *) mt)) {
            printwarning("pthread_create minstack_tcp_accept_thread error\n");
            continue;
        }
    }
    //TODO what is this ?!!!
    //before was //if (mt->pthread_reading_thread && !mt->pthread_reading_thread) {
    if (&mt->pthread_reading_thread && !mt->pthread_reading_thread_stop){
        mt->pthread_reading_thread_stop = 1;
        pthread_join(mt->pthread_reading_thread, NULL);
        printmessage("The reading thread stopped\n");
    }
    printdebug("stopping %s\n",__FUNCTION__);
    pthread_exit(NULL);
    return NULL;
}

/**
 * \brief Boot the minstack_tcp in server mode
 * \param mt the minstack_tcp that have to be boot
 * \return 0 if the minstack_tcp stack boot correctly
 */
int minstack_tcp_boot_server(minstack_tcp *mt) {
    struct sockaddr_in serv_addr;

    if (!mt)
        return -1;
    pthread_mutex_lock(&mt->mutex);
    mt->listen_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (mt->listen_socket_fd < 0) {
        printmoreerror("ERROR opening socket:");
        return -2;
    }
#ifndef WIN32
    if (fcntl(mt->listen_socket_fd, F_SETFL, O_NONBLOCK) < 0) {
        printmoreerror("Could not O_NONBLOCK:");
        return -3;
    }
#else
    {
        u_long imode = 1;
        if(ioctlsocket(mt->listen_socket_fd,FIONBIO,&imode)  < 0) {
            printmoreerror("Could not O_NONBLOCK:");
            return -3;
        }
    }
#endif
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(mt->port);
    if (bind(mt->listen_socket_fd, (const struct sockaddr *) &serv_addr,
            (socklen_t) sizeof(serv_addr)) < 0) {
        printmoreerror("ERROR connecting");
        pthread_mutex_unlock(&mt->mutex);
        return -4;
    }
    listen(mt->listen_socket_fd, (int) mt->max_client_nb);
    if (pthread_create(&mt->pthread_accept_thread, NULL, minstack_tcp_accept_thread, mt)) {
        printwarning("pthread_create minstack_tcp_boot_server error\n");
        pthread_mutex_unlock(&mt->mutex);
        return -5;
    }
    pthread_mutex_unlock(&mt->mutex);
    printdebug("%s started.\n",__FUNCTION__);
    return 0;
}

/**
 * \brief Boot the minstack_tcp in client mode
 * \param mt the minstack_tcp that have to be boot
 * \return 0 if the minstack_tcp stack boot correctly
 */
int minstack_tcp_boot_client(minstack_tcp *mt) {
    struct sockaddr_in serv_addr;
    struct hostent *server;
    if (!mt)
        return -1;

    mt->listen_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (mt->listen_socket_fd < 0) {
        printmoreerror("ERROR opening socket:");
        return -2;
    }
    server = gethostbyname(mt->address);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        return -3;
    }
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
#ifndef WIN32
    bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr,
            server->h_length);
#else
    memcpy( (char *) &serv_addr.sin_addr.s_addr,(char *) server->h_addr,
            server->h_length);
#endif
    serv_addr.sin_port = htons(mt->port);
    if (connect(mt->listen_socket_fd, (const struct sockaddr *) &serv_addr,
            (socklen_t) sizeof(serv_addr)) < 0) {
        printmoreerror("ERROR connecting:");
        return -4;
    }
    if (mt->external_read_socket == NULL) {
        printdebug("We do not start the reading thread in client mode because no external interpretation is done\n");
        mt->pthread_accept_thread_stop = 1;
        mt->pthread_reading_thread_stop = 1;
        return 0;
    }
    mt->sockets.client_socket_fd[0] = mt->listen_socket_fd;
    mt->sockets.connected_client_nb = 1;
    mt->pthread_accept_thread_stop = 1;//because it will not run
    if (pthread_create(&mt->pthread_reading_thread, NULL,
            minstack_tcp_reading_thread, (void *) mt)) {
        printwarning("pthread_create minstack_tcp_accept_thread error\n");
        return -5;
    }
    return 0;
}

/**
 * \brief Start the minstack_tcp in parameter
 * \param mt the minstack_tcp that have to be started
 * \return 0 if the minstack_tcp stack started correctly
 */
int minstack_tcp_start(minstack_tcp *mt) {
    int retval = 0;
    if (!mt || mt->status != IDLE) {
        printerror("Cannot start while already started\n");
        return -1;
    }
    switch (mt->type) {
    case NONE:
        printwarning("the minstack_tcp is not initialized\n");
        return -1;
    case SERVER:
        retval = minstack_tcp_boot_server(mt);
        break;
    case CLIENT:
        retval = minstack_tcp_boot_client(mt);
        break;
    default:
        printerror("Unknow enum type %d\n",mt->type);
        break;
    }
    if (!retval) {
        mt->status = STARTED;
        printmessage("%s is now started\n",mt->name);
    }
    return retval;
}

/**
 * \brief Stop the minstack_tcp in parameter
 * \param mt the minstack_tcp that have to be stopped
 * \return 0 if the minstack_tcp stack stopped correctly
 */
int minstack_tcp_stop(minstack_tcp *mt) {
    if (!mt || mt->status == IDLE) {
        printerror("Cannot stop while not started\n");
        return -1;
    }
#ifdef NOT_CANCEL_THREAD_WHEN_STOPPING
    if (&mt->pthread_accept_thread && !mt->pthread_accept_thread_stop) {
        printdebug("The accepting thread is asked to stop\n");
        mt->pthread_accept_thread_stop = 1;
        pthread_join(mt->pthread_accept_thread, NULL);
        printmessage("The accepting thread stopped\n");
    }
    if (&mt->pthread_reading_thread && !mt->pthread_reading_thread_stop) {
        printdebug("The reading thread is asked to stop\n");
        mt->pthread_reading_thread_stop = 1;
        pthread_join(mt->pthread_reading_thread, NULL);
        printmessage("The reading thread stopped\n");
    }
#else
    if (&mt->pthread_accept_thread && !mt->pthread_accept_thread_stop) {
            printdebug("The accepting thread is asked to stop\n");
            mt->pthread_accept_thread_stop = 1;
            if(mt->pthread_accept_thread)
                pthread_cancel(mt->pthread_accept_thread);
            printmessage("The accepting thread stopped\n");
        }
        if (&mt->pthread_reading_thread && !mt->pthread_reading_thread_stop) {
            printdebug("The reading thread is asked to stop\n");
            mt->pthread_reading_thread_stop = 1;
            if(mt->pthread_reading_thread)
                pthread_cancel(mt->pthread_reading_thread);
            printmessage("The reading thread stopped\n");
        }
#endif
    if (mt->listen_socket_fd)
        minstack_close(mt->listen_socket_fd);
    pthread_mutex_destroy(&mt->mutex);
    mt->status = IDLE;
    printmessage("%s is now stopped\n",mt->name);
    return 0;
}

/**
 * \brief Write the message with the length len_message to the cid
 * \param mt is the minstack_tcp stack
 * \param cid is the client ID to whom send the message
 * \param message the message to send
 * \param len_message the message length
 * \return 0 if the minstack_tcp stack stopped correctly
 */
int minstack_tcp_write(minstack_tcp *mt, int cid, char *message, int len_message) {
    if (!mt) {
        printerror("the minstack_tcp is NULL\n");
        return -100;
    }
    //if (write(cid, message, len_message) < 0) {
    if(send(cid, message, len_message,0) < 0){
        printerror("ERROR writing to socket %d\n",cid);
        return -1;
    }
    printmessage("%s sent:%s length:%d to cid:%d\n",mt->name,message,len_message,cid);
    return 0;
}

/**
 * \brief write a message to a client
 * \param mt is the minstack_tcp stack
 * \param cid is the client ID to whom send the message, if 0 the message is broadcast to the clients
 * \param message is the message
 * \param len_message is the length of the message
 * \return 0 if OK
 */
int minstack_tcp_write_to_client(minstack_tcp *mt, int cid, char *message,
        int len_message) {
    //without cid we send the message to all the clients
    if (!mt) {
        //printerror("the minstack_tcp is NULL\n");
        return -100;
    }
    if (mt->type != SERVER) {
        printwarning("The minstack is not a server...\n");
        return -1;
    }
    if (mt->status != STARTED) {
        printwarning("The server is not started yet\n");
        return -1;
    }
    if (cid)
        return minstack_tcp_write(mt, cid, message, len_message);
    else {
        int i, retval = 0;
        for (i = 0; i < mt->sockets.connected_client_nb; i++) {
            retval = minstack_tcp_write(mt, mt->sockets.client_socket_fd[0],
                    message, len_message);
            if (retval) {
                printerror("We had a problem sending to the client %d\n",i);
                return retval;
            }
        }
    }
    return 0;
}

/**
 * \brief write a message to the server where we are connected
 * \param mt is the minstack_tcp stack
 * \param message is the message
 * \param len_message is the length of the message
 * \return 0 if OK
 */
int minstack_tcp_write_to_server(minstack_tcp *mt, char *message, int len_message) {
    if (!mt) {
        //printerror("the minstack_tcp is NULL\n");
        return -100;
    }
    if (mt->type != CLIENT) {
        printwarning("The minstack is not a server...\n");
        return -1;
    }
    if (mt->status != STARTED) {
        printwarning("The client is not started yet\n");
        return -1;
    }
    return minstack_tcp_write(mt,mt->listen_socket_fd, message, len_message);
}

#define BUF_LEN	(4096)

/**
 * \brief write a message on the minstack_tcp connection
 * \param mt is the minstack_tcp stack
 * \param msg is the message as a classic printf
 */
void minstack_tcp_printf(minstack_tcp *mt, const char *msg, ...) {
    va_list args;
    //char *p;
    char buffer[BUF_LEN];
    int nb_char_written;

    if (!mt) {
        //printerror("the minstack_tcp is NULL\n");
        return;
    }
    if ((mt->type != CLIENT && mt->type != SERVER) || mt->status != STARTED)
        return;

    va_start(args, msg);
    nb_char_written = vsnprintf(buffer, BUF_LEN, msg, args);
    va_end(args);

    if (nb_char_written == -1)
        strcpy(buffer, "internal vsnprintf error");

    /*
    // remplace d'enventuels \n par un | (sauf en fin de buffer)
    p = buffer;
    while (*p != '\0') {
        if (*p == '\n')
            *p = '|';
        p++;
    }
    if (*(p - 1) == '|')
        *(p - 1) = '\0';
    */
    if (mt->type == CLIENT)
        minstack_tcp_write_to_server(mt, buffer, nb_char_written);
    else
        minstack_tcp_write_to_client(mt, 0, buffer, nb_char_written);
}

/**
 * \brief Generate a minstack_tcp
 * \param nickname is the name given to the minstack_tcp could be NULL
 * \return the minstack_tcp generated
 */
minstack_tcp * minstack_tcp_init(const char *nickname) {
#ifdef WIN32
    win32_init_socket_api();
#endif
    minstack_tcp *mt = (minstack_tcp *) calloc(1, sizeof(minstack_tcp));
    if (mt == NULL) {
        printerror("Could not allocate a minstack_tcp\n");
        return NULL;
    }
    mt->name = nickname;
    mt->type = NONE;

    mt->status = IDLE;
    mt->pthread_reading_thread_stop = 1;
    printmessage("%s minstack TCP has been initialized\n",mt->name);
    return mt;
}

/**
 * \brief Uninitialized a minstack_tcp
 * \param mt is the minstack_tcp stack
 */
void minstack_tcp_uninit(minstack_tcp *mt) {
    if (!mt) {
        printwarning("Trying to uninit a NULL minstack_tcp\n");
        return;
    }
    if (mt->status == STARTED) {
        minstack_tcp_stop(mt);
        printdebug("%s minstack has TCP been stopped before uninit\n",mt->name);
    }
    printmessage("%s minstack TCP has been uninitialized\n",mt->name);
    free(mt);
#ifdef WIN32
    win32_uninit_socket_api();
#endif
}

/**
 * \brief Init a minstack_tcp stack as a server
 * \param mt is the minstack_tcp stack
 * \param port is the port where the client could try to connect
 * \param max_client_number is the maximum number of clients that could be connected
 * \return 0 if OK
 */
int minstack_tcp_init_server(minstack_tcp *mt, int port,
        unsigned int max_client_number) {
    if (!mt || mt->type != NONE)
        return -1;
    pthread_mutex_init(&mt->mutex, NULL);
    pthread_mutex_lock(&mt->mutex);
    mt->type = SERVER;
    mt->port = port;
    mt->address = "127.0.0.1";//not used actually in server mode
    mt->receive_loop_usleep = 100 * 1000;
    mt->max_client_nb = max_client_number;
    mt->new_connection_callback = minstack_default_new_connection_callback;
    mt->connection_closed_callback
            = minstack_default_connection_closed_server_callback;
    //mt->read_socket = minstack_tcp_default_read;
    mt->read_socket = minstack_tcp_recvfrom_read;
    mt->external_read_socket = NULL;
    pthread_mutex_unlock(&mt->mutex);
    return 0;
}

/**
 * \brief Init a minstack_tcp stack as a client
 * \param mt is the minstack_tcp stack
 * \param port is the port where the client has to connect
 * \param address is the IP address where the client has to be connected
 * \return 0 if OK
 */
int minstack_tcp_init_client(minstack_tcp *mt, int port, const char *address) {
    if (!mt || mt->type != NONE)
        return -1;
    mt->type = CLIENT;
    mt->port = port;
    mt->address = address;
    mt->receive_loop_usleep = 100 * 1000;
    mt->new_connection_callback = NULL;
    mt->connection_closed_callback = NULL;
    //mt->read_socket = minstack_tcp_default_read;
    mt->read_socket = minstack_tcp_recvfrom_read;
    mt->external_read_socket = NULL;
    pthread_mutex_init(&mt->mutex, NULL);
    return 0;
}

/**
 * \brief Set the time taken be the receive loop
 * \param mt is the minstack_tcp stack
 * \param usleepvalue is the time in micro seconds
 */
void minstack_tcp_set_receive_loop_usleep(minstack_tcp *mt,
        unsigned int usleepvalue) {
    if (mt)
        mt->receive_loop_usleep = usleepvalue;
}

/**
 * \brief Get the time taken be the receive loop
 * \param mt is the minstack_tcp stack
 * \return the time in micro seconds
 */
unsigned int minstack_tcp_get_receive_loop_usleep(minstack_tcp *mt) {
    if (mt)
        return mt->receive_loop_usleep;
    return 0;
}

/**
 * \brief Set the external function call when something is received
 * \param mt is the minstack_tcp stack
 * \param function is the function call when something is received
 * \return 0 if OK
 */
int minstack_tcp_set_external_read_function(minstack_tcp *mt, void(*function)(
        int cid, const char *from, char *buffer, unsigned int buffer_size_returned)) {
    if (!mt || mt->type == NONE)
        return -1;
    mt->external_read_socket = function;
    return 0;
}

/**
 * \brief The simplest way to start a client
 * \param nickname is the name given to the minstack_tcp could be NULL
 * \param port is the port where the client has to connect
 * \param address is the IP address where the client has to be connected
 * \return the minstack_tcp stack generated, NULL if something wrong happenned
 */
minstack_tcp *minstack_tcp_start_a_client(const char *nickname, int port,
        const char *address) {
    int retval;
    minstack_tcp *mt = minstack_tcp_init(nickname);
    if (mt == NULL)
    {
        printerror("Cannot init a minstack TCP\n");
        return NULL;
    }
    retval = minstack_tcp_init_client(mt, port, address);
    if (retval) {
        printerror("Cannot init TCP client\n");
        minstack_tcp_uninit(mt);
        return NULL;
    }
    retval = minstack_tcp_start(mt);
    if (retval) {
        printerror("Cannot start TCP client\n");
        minstack_tcp_uninit(mt);
        return NULL;
    }
    return mt;
}

/**
 * \brief The simplest way to start a server
 * \param nickname is the name given to the minstack_tcp could be NULL
 * \param port is the port where the client could try to connect
 * \param max_client_number is the maximum number of clients that could be connected
 * \return the minstack_tcp stack generated, NULL if something wrong happenned
 */
minstack_tcp *minstack_tcp_start_a_server(const char *nickname, int port,
        int max_client_number) {
    int retval;
    minstack_tcp *mt = minstack_tcp_init(nickname);
    if (mt == NULL)
    {
        printerror("Cannot init a minstack TCP\n");
        return NULL;
    }
    retval = minstack_tcp_init_server(mt, port, max_client_number);
    if (retval) {
        printerror("Cannot init TCP server\n");
        minstack_tcp_uninit(mt);
        return NULL;
    }
    retval = minstack_tcp_start(mt);
    if (retval) {
        printerror("Cannot start TCP server\n");
        minstack_tcp_uninit(mt);
        return NULL;
    }
    return mt;
}

/**
 * \brief The simplest way to start a server with a special read function
 * \param nickname is the name given to the minstack_tcp could be NULL
 * \param port is the port where the client could try to connect
 * \param max_client_number is the maximum number of clients that could be connected
 * \param function is the function call when something is received
 * \return the minstack_tcp stack generated, NULL if something wrong happenned
 */
minstack_tcp *minstack_tcp_start_a_server_with_read_function(
        const char *nickname, int port, int max_client_number, void(*function)(
                int cid, const char *from, char *buffer, unsigned int buffer_size_returned)) {
    int retval;
    minstack_tcp *mt = minstack_tcp_init(nickname);
    if (mt == NULL)
        return NULL;
    retval = minstack_tcp_init_server(mt, port, max_client_number);
    if (retval) {
        minstack_tcp_uninit(mt);
        return NULL;
    }
    retval = minstack_tcp_set_external_read_function(mt, function);
    if (retval) {
        minstack_tcp_uninit(mt);
        return NULL;
    }
    retval = minstack_tcp_start(mt);
    if (retval) {
        minstack_tcp_uninit(mt);
        return NULL;
    }
    return mt;
}

int minstack_tcp_recvfrom_read(int cid, char *from, char **buffer) {
	int buffer_size_returned=0;
    int retval, finished = 0;
    char read_buffer[DEFAULT_READ_BUFFER_SIZE] = { 0 };
    unsigned int buffer_size = DEFAULT_READ_BUFFER_SIZE;
    struct sockaddr_storage their_addr;
    socklen_t addr_len;
    char s[INET6_ADDRSTRLEN];

    printdebug("There is something that is going to be read\n");
    if (*buffer != NULL) {
    	printerror("You have to give a NULL pointer for buffer\n");
    	return -1;
    }
    if (from == NULL) {
        printerror("You have to give a char tab for from\n");
        return -2;
    }
    *buffer = (char *) calloc(1, buffer_size);
    if (!*buffer) {
        printerror("We could not get enough memory to allocate %d octets to read\n",sizeof(read_buffer));
        buffer_size_returned = 0;
        return -4;
    }

    while (!finished) {
    	int ret_getpeername=0;
        //retval = read(cid,read_buffer,DEFAULT_READ_BUFFER_SIZE);
        retval = recv(cid, read_buffer, DEFAULT_READ_BUFFER_SIZE, MSG_DONTWAIT);
        printdebug("recv returned %d\n",retval);
        addr_len = sizeof(their_addr);
        ret_getpeername = getpeername(cid, (struct sockaddr *)&their_addr, &addr_len);
        printdebug("getpeername returned %d:%s\n",ret_getpeername,strerror(errno));
        if (retval > 0 && retval < DEFAULT_READ_BUFFER_SIZE) {
            memcpy(*buffer + (buffer_size - DEFAULT_READ_BUFFER_SIZE),
                    read_buffer, DEFAULT_READ_BUFFER_SIZE);
            finished = 1;
        } else if (retval == DEFAULT_READ_BUFFER_SIZE) {
            char *new_buffer;
            memcpy(*buffer + (buffer_size - DEFAULT_READ_BUFFER_SIZE),
                    read_buffer, DEFAULT_READ_BUFFER_SIZE);
            new_buffer = (char *) calloc(1, buffer_size
                    +DEFAULT_READ_BUFFER_SIZE);
            if (!new_buffer) {
                printmoreerror("we had a problem to read when moving buffers");
                free(*buffer);
                buffer_size_returned = 0;
                return -5;
            }
            /*
             new_buffer = strncpy(new_buffer,buffer,buffer_size);
             char *p_next_char = new_buffer + buffer_size;
             strncpy(p_next_char,read_buffer,DEFAULT_READ_BUFFER_SIZE);
             */
            memcpy(new_buffer, *buffer, buffer_size);
            buffer_size += DEFAULT_READ_BUFFER_SIZE;
            free(*buffer);
            *buffer = new_buffer;
            finished = 0;
        } else if (retval < 0) {
            //printmoreerror("we had a problem to read");
            printdebug("we had a problem to read\n");
            free(*buffer);
            buffer_size_returned = 0;
            return buffer_size_returned;
        } else if (retval == 0) {
            //the client just disconnected
            if (*buffer)
                free(*buffer);
            buffer_size_returned = -1;
            return buffer_size_returned;
        }
        if(retval > 0){
        	snprintf(from,16,"%s",inet_ntop(their_addr.ss_family,
        	            get_in_addr((struct sockaddr *)&their_addr),
        	            s, sizeof s));
        	printdebug("Get datas from %s\n",from);
        }
    }
    buffer_size_returned = buffer_size;
    return buffer_size_returned;
}

char *minstack_tcp_default_read(int cid, unsigned int *buffer_size_returned) {
    int retval, finished = 0;
    char read_buffer[DEFAULT_READ_BUFFER_SIZE] = { 0 };
    unsigned int buffer_size = DEFAULT_READ_BUFFER_SIZE;
    char *buffer;

    printdebug("There is something that is going to be read\n");
    if (!buffer_size_returned) {
        printerror("You have to give a pointer to buffer_size_returned\n");
        return NULL;
    }
    buffer = (char *) calloc(1, buffer_size);

    if (!buffer) {
        printerror("We could not get enough memory to allocate %d octets to read\n",sizeof(read_buffer));
        *buffer_size_returned = 0;
        return NULL;
    }

    while (!finished) {
        //retval = read(cid,read_buffer,DEFAULT_READ_BUFFER_SIZE);
        retval = recv(cid, read_buffer, DEFAULT_READ_BUFFER_SIZE, MSG_DONTWAIT);
        printdebug("recv returned %d\n",retval);
        if (retval > 0 && retval < DEFAULT_READ_BUFFER_SIZE) {
            memcpy(buffer + (buffer_size - DEFAULT_READ_BUFFER_SIZE),
                    read_buffer, DEFAULT_READ_BUFFER_SIZE);
            finished = 1;
        } else if (retval == DEFAULT_READ_BUFFER_SIZE) {
            char *new_buffer;
            memcpy(buffer + (buffer_size - DEFAULT_READ_BUFFER_SIZE),
                    read_buffer, DEFAULT_READ_BUFFER_SIZE);
            new_buffer = (char *) calloc(1, buffer_size
                    +DEFAULT_READ_BUFFER_SIZE);
            if (!new_buffer) {
                printmoreerror("we had a problem to read when moving buffers");
                free(buffer);
                *buffer_size_returned = 0;
                return NULL;
            }
            /*
             new_buffer = strncpy(new_buffer,buffer,buffer_size);
             char *p_next_char = new_buffer + buffer_size;
             strncpy(p_next_char,read_buffer,DEFAULT_READ_BUFFER_SIZE);
             */
            memcpy(new_buffer, buffer, buffer_size);
            buffer_size += DEFAULT_READ_BUFFER_SIZE;
            free(buffer);
            buffer = new_buffer;
            finished = 0;
        } else if (retval < 0) {
            //printmoreerror("we had a problem to read");
            printdebug("we had a problem to read\n");
            free(buffer);
            *buffer_size_returned = 0;
            return NULL;
        } else if (retval == 0) {
            //the client just disconnected
            if (buffer)
                free(buffer);
            *buffer_size_returned = -1;
            return NULL;
        }
    }
    *buffer_size_returned = buffer_size;
    return buffer;
}

