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

#include "arguments.h"
#include "estat_client.h"
#include "subscripcio.h"
#include "socket.h"

int subscripcio(Estat* estat_client, Configuracio* configuracio) {
    estat_client->estat = DISCONNECTED;

    Socket* socket = (Socket*) malloc(sizeof (Socket));
    if ((socket->he = gethostbyname(configuracio->server)) == NULL) {
        /* llamada a gethostbyname() */
        fprintf(stderr, "gethostbyname() error\n");
        return -1;
    }

    if ((socket->fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        /* llamada a socket() */
        fprintf(stderr, "socket() error\n");
        return -1;
    }

    socket->server.sin_family = AF_INET;
    socket->server.sin_port = htons(configuracio->srv_udp);
    struct hostent* he = socket->he;
    (socket->server).sin_addr = *((struct in_addr *) he->h_addr );
    bzero(&(configuracio->server.sin_zero), 8);



    return 0;
}