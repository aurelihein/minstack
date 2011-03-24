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
#include "udp.h"

int minstack_udp_boot_server(minstack_udp *mu);
int minstack_udp_boot_client(minstack_udp *mu);
char *minstack_udp_default_read(int cid, unsigned int *buffer_size_returned);

/**
 * \brief Generate a minstack_udp
 * \param nickname is the name given to the minstack_udp could be NULL
 * \return the minstack_udp generated
 */
minstack_udp * minstack_udp_init(const char *nickname) {
#ifdef WIN32
    win32_init_socket_api();
#endif
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
#ifdef WIN32
    win32_uninit_socket_api();
#endif
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
    /*
    mu->new_connection_callback = minstack_default_new_connection_callback;
    mu->connection_closed_callback = minstack_default_connection_closed_server_callback;
    */
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
    /*
    mu->new_connection_callback = NULL;
    mu->connection_closed_callback = NULL;
    mu->read_socket = minstack_udp_default_read;
    mu->external_read_socket = NULL;
    */
    pthread_mutex_init(&mu->mutex, NULL);
    return 0;
}

/**
 * \brief Start the minstack_udp in parameter
 * \param mu the minstack_udp that have to be started
 * \return 0 if the minstack_udp stack started correctly
 */
int minstack_udp_start(minstack_udp *mu) {
    int retval = 0;
    if (!mu || mu->status != IDLE) {
        printerror("Cannot start while already started\n");
        return -1;
    }
    switch (mu->type) {
    case NONE:
        printwarning("the minstack_udp is not initialized\n");
        return -1;
    case SERVER:
        retval = minstack_udp_boot_server(mu);
        break;
    case CLIENT:
        retval = minstack_udp_boot_client(mu);
        break;
    default:
        printerror("Unknow enum type %d\n",mu->type);
    }
    if (!retval) {
        mu->status = STARTED;
        printmessage("%s is now started\n",mu->name);
    }
    return retval;
}

/**
 * \brief The simplest way to start a client
 * \param nickname is the name given to the minstack_udp could be NULL
 * \param port is the port where the client has to connect
 * \param address is the IP address where the client has to be connected
 * \return the minstack_udp stack generated, NULL if something wrong happenned
 */
minstack_udp *minstack_udp_start_a_client(const char *nickname, int port, const char *address) {
    int retval;
    minstack_udp *mu = minstack_udp_init(nickname);
    if (mu == NULL)
        return NULL;
    retval = minstack_udp_init_client(mu, port, address);
    if (retval) {
        minstack_udp_uninit(mu);
        return NULL;
    }
    retval = minstack_udp_start(mu);
    if (retval) {
        minstack_udp_uninit(mu);
        return NULL;
    }
    return mu;
}

/**
 * \brief The simplest way to start a server
 * \param nickname is the name given to the minstack_udp could be NULL
 * \param port is the port where the client could try to connect
 * \param max_client_number is the maximum number of clients that could be connected
 * \return the minstack_udp stack generated, NULL if something wrong happenned
 */
minstack_udp *minstack_udp_start_a_server(const char *nickname, int port, int max_client_number) {
    int retval;
    minstack_udp *mu = minstack_udp_init(nickname);
    if (mu == NULL)
        return NULL;
    retval = minstack_udp_init_server(mu, port, max_client_number);
    if (retval) {
        minstack_udp_uninit(mu);
        return NULL;
    }
    retval = minstack_udp_start(mu);
    if (retval) {
        minstack_udp_uninit(mu);
        return NULL;
    }
    return mu;
}

/**
 * \brief The simplest way to start a server with a special read function
 * \param nickname is the name given to the minstack_udp could be NULL
 * \param port is the port where the client could try to connect
 * \param max_client_number is the maximum number of clients that could be connected
 * \param function is the function call when something is received
 * \return the minstack_udp stack generated, NULL if something wrong happenned
 */
minstack_udp *minstack_udp_start_a_server_with_read_function(
        const char *nickname, int port, int max_client_number, void(*function)(
                int cid, char *buffer, unsigned int buffer_size_returned)) {
    int retval;
    minstack_udp *mu = minstack_udp_init(nickname);
    if (mu == NULL)
        return NULL;
    retval = minstack_udp_init_server(mu, port, max_client_number);
    if (retval) {
        minstack_udp_uninit(mu);
        return NULL;
    }
    retval = minstack_udp_set_external_read_function(mu, function);
    if (retval) {
        minstack_udp_uninit(mu);
        return NULL;
    }
    retval = minstack_udp_start(mu);
    if (retval) {
        minstack_udp_uninit(mu);
        return NULL;
    }
    return mu;
}

#define FD_SETMAX(s, ss, m) { FD_SET((s), ss); if ((s) > m) m = (s); }
void *minstack_udp_reading_thread(void *ptr) {
    minstack_udp *mu = (minstack_udp *) ptr;
    fd_set fds_read;
    int fdmax;
    struct timeval tv;

    printdebug("starting %s\n",__FUNCTION__);
    //pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    //pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    mu->pthread_reading_thread_stop = 0;
    while (!mu->pthread_reading_thread_stop) {
        int retval;
        fdmax = 0;
        tv.tv_sec = 0;
        tv.tv_usec = (mu->receive_loop_usleep);
        FD_ZERO(&fds_read);
        FD_SETMAX(mu->listen_socket_fd,&fds_read,fdmax);
        fdmax++;
        retval = select(fdmax, &fds_read, NULL, NULL, &tv);
        pthread_mutex_lock(&mu->mutex);
        if (retval == EBADF) {
            printmoreerror("Select Bad FD\n");
            continue;
        }
        if (retval < 0) {
            printerror("Select: got an error %d\n",retval);
            printmoreerror("Select:");
            break;
        }
        if (!retval) {
            //printmoreerror("Nothing much from the select\n");
        }
        pthread_mutex_unlock(&mu->mutex);
        if (retval > 0) {
            char *buffer;
            unsigned int buffer_size = 0;
            pthread_mutex_lock(&mu->mutex);
            pthread_mutex_unlock(&mu->mutex);
            buffer = mu->read_socket(mu->listen_socket_fd, &buffer_size);

            if (buffer_size == -1) {
                printmessage("The buffer size returned is -1 is it possible ?\n");
            } else {
                printmessage("%s received from %d(%u)=>%s\n",mu->name, mu->listen_socket_fd,buffer_size,buffer);
                if (mu->external_read_socket)
                    mu->external_read_socket(mu->listen_socket_fd, buffer, buffer_size);
                free(buffer);
            }
        }
    }
    mu->pthread_reading_thread_stop = 1;
    printdebug("stopping %s\n",__FUNCTION__);
    pthread_exit(NULL);
    return NULL;
}

/**
 * \brief Boot the minstack_udp in server mode
 * \param mu the minstack_udp that have to be boot
 * \return 0 if the minstack_udp stack boot correctly
 */
int minstack_udp_boot_server(minstack_udp *mu) {
    struct sockaddr_in serv_addr;

    if (!mu)
        return -1;
    pthread_mutex_lock(&mu->mutex);
    mu->listen_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (mu->listen_socket_fd < 0) {
        printmoreerror("ERROR opening socket");
        return -2;
    }
    /*
    if (fcntl(mu->listen_socket_fd, F_SETFL, O_NONBLOCK) < 0) {
        printmoreerror("Could not O_NONBLOCK");
        return -3;
    }
    */
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(mu->port);
    if (bind(mu->listen_socket_fd, (const struct sockaddr *) &serv_addr,
            (socklen_t) sizeof(serv_addr)) < 0) {
        printmoreerror("ERROR connecting");
        pthread_mutex_unlock(&mu->mutex);
        return -4;
    }

    listen(mu->listen_socket_fd, (int) mu->max_client_nb);
    if (pthread_create(&mu->pthread_reading_thread, NULL, minstack_udp_reading_thread, mu)) {
        printwarning("pthread_create minstack_udp_boot_server error\n");
        pthread_mutex_unlock(&mu->mutex);
        return -5;
    }
    pthread_mutex_unlock(&mu->mutex);
    printdebug("%s started.\n",__FUNCTION__);
    return 0;
}

/**
 * \brief Boot the minstack_udp in client mode
 * \param mu the minstack_udp that have to be boot
 * \return 0 if the minstack_udp stack boot correctly
 */
int minstack_udp_boot_client(minstack_udp *mu) {
    struct sockaddr_in serv_addr;
    struct hostent *server;
    if (!mu)
        return -1;

    mu->listen_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (mu->listen_socket_fd < 0) {
        printmoreerror("ERROR opening socket");
        return -2;
    }
    server = gethostbyname(mu->address);
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
    memcpy((char *) &serv_addr.sin_addr.s_addr,(char *) server->h_addr,
            server->h_length);
#endif
    serv_addr.sin_port = htons(mu->port);
    if (connect(mu->listen_socket_fd, (const struct sockaddr *) &serv_addr,
            (socklen_t) sizeof(serv_addr)) < 0) {
        printmoreerror("ERROR connecting");
        return -4;
    }
    mu->pthread_reading_thread_stop = 1;
    return 0;
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
    if (&mu->pthread_reading_thread && !mu->pthread_reading_thread_stop) {
        printdebug("The reading thread is asked to stop\n");
        mu->pthread_reading_thread_stop = 1;
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
 * \brief Set the external function call when something is received
 * \param mu is the minstack_udp stack
 * \param function is the function call when something is received
 * \return 0 if OK
 */
int minstack_udp_set_external_read_function(minstack_udp *mu, void(*function)(
        int cid, char *buffer, unsigned int buffer_size_returned)) {
    if (!mu || mu->type != SERVER)
        return -1;
    mu->external_read_socket = function;
    return 0;
}

/**
 * \brief write a message to the server where we are connected
 * \param mu is the minstack_udp stack
 * \param message is the message
 * \param len_message is the length of the message
 * \return 0 if OK
 */
int minstack_udp_write_to_server(minstack_udp *mu, char *message, int len_message) {
    if (!mu) {
        //printerror("the minstack_udp is NULL\n");
        return -100;
    }
    if (mu->type != CLIENT) {
        printwarning("The minstack is not a server...\n");
        return -1;
    }
    if (mu->status != STARTED) {
        printwarning("The client is not started yet \n");
        return -1;
    }
    if (write(mu->listen_socket_fd, message, len_message) < 0) {
        printerror("ERROR writing to server socket %d",mu->listen_socket_fd);
        return -2;
    }
    printmessage("sent:%s length:%d to cid:%d\n",message,len_message,mu->listen_socket_fd);
    return 0;
}

#define BUF_LEN (4096)

/**
 * \brief write a message on the minstack_udp connection
 * \param mu is the minstack_udp stack
 * \param msg is the message as a classic printf
 */
void minstack_udp_printf(minstack_udp *mu, const char *msg, ...) {
    va_list args;
    char *p, buffer[BUF_LEN];
    int nb_char_written;

    if (!mu) {
        //printerror("the minstack_udp is NULL\n");
        return;
    }
    if ((mu->type != CLIENT ) || mu->status != STARTED)
    {
        printerror("The minstack UDP has to be a started client to printf\n");
        return;
    }

    va_start(args, msg);
    nb_char_written = vsnprintf(buffer, BUF_LEN, msg, args);
    va_end(args);

    if (nb_char_written == -1)
        strcpy(buffer, "internal vsnprintf error");

    // remplace d'enventuels \n par un | (sauf en fin de buffer)
    p = buffer;
    while (*p != '\0') {
        if (*p == '\n')
            *p = '|';
        p++;
    }
    if (*(p - 1) == '|')
        *(p - 1) = '\0';
        minstack_udp_write_to_server(mu, buffer, nb_char_written);
}

char *minstack_udp_default_read(int cid, unsigned int *buffer_size_returned) {
    int retval, finished = 0;
    char read_buffer[DEFAULT_READ_BUFFER_SIZE] = { 0 };
    unsigned int buffer_size = DEFAULT_READ_BUFFER_SIZE;
    char *buffer;

    printdebug("there is something that is going to be read\n");
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
            printdebug("we had a problem to read");
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
