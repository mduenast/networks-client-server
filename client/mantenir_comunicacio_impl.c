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

#include "estat_client.h"
#include "mantenir_comunicacio.h"
#include "socket_client.h"
#include "stdlib.h"
#include "unistd.h"
#include "arguments.h"
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>


int start_socket_mantenir_comunicacio(Socket_client* socket_client, Configuracio* configuracio) {
    socket_client->he = gethostbyname(configuracio->server);
    if (socket_client->he == NULL) {
        fprintf(stderr, "gethostbyname() error\n");
        return -1;
    }
    socket_client->fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_client->fd == -1) {
        fprintf(stderr, "socket() error\n");
        return -1;
    }
    socket_client->server.sin_family = AF_INET;
    socket_client->server.sin_port = htons(configuracio->srv_udp);
    socket_client->server.sin_addr = *((struct in_addr*) socket_client->he->h_addr_list[0]);
    memset(&(socket_client->server.sin_zero), 0, sizeof (socket_client->server.sin_zero));
    return 0;
}

void prepara_pdu_mantenir_comunicacio(PDU_comunicacio* pdu, unsigned char tipus_paquet, Configuracio* configuracio, char* numero_aleatori, char* dades) {
    pdu->tipus_paquet = tipus_paquet;
    strcpy(pdu->mac, configuracio->mac);
    strcpy(pdu->numero_aleatori, numero_aleatori);
    strcpy(pdu->dades, dades);
}

int comprova_dades_mantenir_comunicacio(Estat* estat_client, Configuracio* configuracio, Socket_client* socket_client, PDU_comunicacio* pdu) {
    if (strcmp(configuracio->dades_servidor.ip, inet_ntoa(socket_client->server.sin_addr)) != 0) {
        return -1;
    }
    if (strcmp(configuracio->dades_servidor.mac, pdu->mac) != 0) {
        return -1;
    }
    if (strcmp(configuracio->dades_servidor.numero_aleatori, pdu->numero_aleatori) != 0) {
        return -1;
    }
    return 0;
}

int mantenir_comunicacio(Estat* estat_client, Configuracio* configuracio){
    
    return 0;
}