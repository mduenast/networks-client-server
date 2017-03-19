/*
 * Copyright (C) 2017 mdt3
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SOCKET_H
#define SOCKET_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <string.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>  

typedef struct socket_client {
    int fd;
    struct hostent *he;
    struct sockaddr_in server;
} Socket_client;

typedef struct socket_client_enviar_dades {
    int fd;
    struct hostent *he;
    struct sockaddr_in server;
} Socket_client_enviar_dades;

typedef struct socket_client_rebre_dades {
    int fd_client;
    int fd_server;
    struct sockaddr_in client;
    struct sockaddr_in server;
} Socket_client_rebre_dades;

#endif /* SOCKET_H */

