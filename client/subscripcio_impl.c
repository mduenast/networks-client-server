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
#include "socket_client.h"
#include "stdlib.h"
#include "unistd.h"
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>

int subscripcio(Estat* estat_client, Configuracio* configuracio) {
    Socket_client* socket_client = (Socket_client*) malloc(sizeof (Socket_client));
    if (start_socket(socket_client, configuracio) == -1) {
        return -1;
    }
    // prepara el paquet
    PDU* pdu = (PDU*) malloc(sizeof (PDU));
    char dades[80];
    sprintf(dades, "%s,%s", configuracio->name, configuracio->situation);
    prepara_pdu(pdu, SUBS_REQ, configuracio, "00000000", dades);
    // envia el paquet
    envia(estat_client, socket_client, pdu);
    estat_client->estat = WAIT_ACK_SUBS;
    printf("INFO => El client passa a estat WAIT_ACK_SUBS\n");

    fd_set read_set;
    FD_ZERO(&read_set);
    FD_SET(socket_client->fd, &read_set);

    struct timeval time_out;
    time_out.tv_sec = T;
    time_out.tv_usec = 0;
    int result = select(socket_client->fd + 1, &read_set, NULL, NULL, &time_out);
    asynchronous_read(estat_client, socket_client, configuracio, pdu, &read_set, result);
    int i, j;
    int paquets = 1;
    for (j = 0; j < O && estat_client->estat != SUBSCRIBED; j++) {
        for (i = 1; i < P && estat_client->estat != SUBSCRIBED; i++) {
            time_out.tv_sec = T;
            if (estat_client->estat == WAIT_ACK_SUBS) {
                envia(estat_client, socket_client, pdu);
            }
            FD_ZERO(&read_set);
            FD_SET(socket_client->fd, &read_set);
            result = select(socket_client->fd + 1, &read_set, NULL, NULL, &time_out);
            asynchronous_read(estat_client, socket_client, configuracio, pdu, &read_set, result);
            paquets++;
        }
        for (i = 2; i < Q && estat_client->estat != SUBSCRIBED; i++) {
            time_out.tv_sec = T * i;
            if (estat_client->estat == WAIT_ACK_SUBS) {
                envia(estat_client, socket_client, pdu);
            }
            FD_ZERO(&read_set);
            FD_SET(socket_client->fd, &read_set);
            result = select(socket_client->fd + 1, &read_set, NULL, NULL, &time_out);
            asynchronous_read(estat_client, socket_client, configuracio, pdu, &read_set, result);
            paquets++;
        }
        time_out.tv_sec = Q * T;
        while (paquets <= N && estat_client->estat != SUBSCRIBED) {
            if (estat_client->estat == WAIT_ACK_SUBS) {
                envia(estat_client, socket_client, pdu);
            }
            FD_ZERO(&read_set);
            FD_SET(socket_client->fd, &read_set);
            result = select(socket_client->fd + 1, &read_set, NULL, NULL, &time_out);
            asynchronous_read(estat_client, socket_client, configuracio, pdu, &read_set, result);
            paquets++;
        }
        paquets = 0;
        time_out.tv_sec = U;
        FD_ZERO(&read_set);
        FD_SET(socket_client->fd, &read_set);
        result = select(socket_client->fd + 1, &read_set, NULL, NULL, &time_out);
        asynchronous_read(estat_client, socket_client, configuracio, pdu, &read_set, result);
    }
    close(socket_client->fd);
    if(j == O){
        fprintf(stderr,"SEVERE => No s'ha pogut establir connexio amb el servidor\n");
        exit(EXIT_FAILURE);
    }
    return 0;
}

int start_socket(Socket_client* socket_client, Configuracio* configuracio) {
    socket_client->he = gethostbyname(configuracio->server);
    if (socket_client->he == NULL) {
        fprintf(stderr, "SEVERE => gethostbyname() error\n");
        return -1;
    }
    socket_client->fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_client->fd == -1) {
        fprintf(stderr, "SEVERE => socket() error\n");
        return -1;
    }
    socket_client->server.sin_family = AF_INET;
    socket_client->server.sin_port = htons(configuracio->srv_udp);
    socket_client->server.sin_addr = *((struct in_addr*) socket_client->he->h_addr_list[0]);
    memset(&(socket_client->server.sin_zero), 0, sizeof (socket_client->server.sin_zero));
    return 0;
}

void prepara_pdu(PDU* pdu, unsigned char tipus_paquet, Configuracio* configuracio, char* numero_aleatori, char* dades) {
    pdu->tipus_paquet = tipus_paquet;
    strcpy(pdu->mac, configuracio->mac);
    strcpy(pdu->numero_aleatori, numero_aleatori);
    strcpy(pdu->dades, dades);
}

void envia(Estat* estat_client, Socket_client* socket_client, PDU* pdu) {
    int bytes = sendto(socket_client->fd, pdu, sizeof (PDU), 0,
            (struct sockaddr*) &(socket_client->server), sizeof (struct sockaddr));
    if(estat_client->debug == 1){
        printf("DEBUG => Envia { tipus paquet : %c , mac : %s , numero aleatori : %s , dades : %s }\n",
                pdu->tipus_paquet, pdu->mac, pdu->numero_aleatori, pdu->dades);
    }
    if (bytes == -1) {
        fprintf(stderr, "SEVERE => sendto() error\n");
    }
}

void rep_resposta(Estat* estat_client, Configuracio* configuracio, Socket_client* socket_client, PDU* pdu) {
    socklen_t socklen = sizeof (struct sockaddr);
    int bytes = recvfrom(socket_client->fd, pdu, sizeof (PDU), 0, (struct sockaddr*) &(socket_client->server), &socklen);
    if(estat_client->debug == 1){
        printf("DEBUG => Rep { tipus paquet : %c , mac : %s , numero aleatori : %s , dades : %s }\n",
                pdu->tipus_paquet, pdu->mac, pdu->numero_aleatori, pdu->dades);
    }
    if (bytes == -1) {
        fprintf(stderr, "SEVERE => recvfrom() error\n");
    }
    comprova_resposta(estat_client, configuracio, socket_client, pdu);
}

void comprova_resposta(Estat* estat_client, Configuracio* configuracio, Socket_client* socket_client, PDU* pdu) {
    if (pdu->tipus_paquet == SUBS_ACK && estat_client->estat == WAIT_ACK_SUBS) {
        strcpy(configuracio->dades_servidor.ip, inet_ntoa(socket_client->server.sin_addr));
        strcpy(configuracio->dades_servidor.mac, pdu->mac);
        strcpy(configuracio->dades_servidor.numero_aleatori, pdu->numero_aleatori);
        configuracio->dades_servidor.udp_port = atoi(pdu->dades);

        char dades[80];
        memset(dades, 0, sizeof (dades));
        sprintf(dades, "%i,", configuracio->local_tcp);
        int i;
        for (i = 0; i < (sizeof (configuracio->elements) / sizeof (Element))
                && strlen(configuracio->elements[i].codi); i++) {
            strcat(dades, configuracio->elements[i].codi);
            strcat(dades, ";");
        }
        prepara_pdu(pdu, SUBS_INFO, configuracio, configuracio->dades_servidor.numero_aleatori, dades);
        socket_client->server.sin_port = htons(configuracio->dades_servidor.udp_port);
        envia(estat_client, socket_client, pdu);
        estat_client->estat = WAIT_ACK_INFO;
        printf("INFO => El client passa al estat WAIT_ACK_INFO\n");
    } else if (pdu->tipus_paquet == SUBS_NACK || pdu->tipus_paquet == SUBS_REJ) {
        estat_client->estat = NOT_SUBSCRIBED;
        printf("INFO => El client passa a estat NOT_SUBSCRIBED\n");
        fprintf(stderr,"SEVERE => El servidor ha rebutjat la conexio\n");
        subscripcio(estat_client,configuracio);
    } else if (pdu->tipus_paquet == INFO_ACK && estat_client->estat == WAIT_ACK_INFO) {
        if (comprova_dades(estat_client, configuracio, socket_client, pdu) == 0) {
            configuracio->tcp_enviar_rebre_dades = atoi(pdu->dades);
            estat_client->estat = SUBSCRIBED;
            printf("INFO => El client passa a estat SUBSCRIBED\n");
        } else {
            estat_client->estat = NOT_SUBSCRIBED;
            printf("INFO => El client passa a estat NOT_SUBSCRIBED\n");
            fprintf(stderr,"SEVERE => Les dades no han pogut estar validades\n");
            subscripcio(estat_client,configuracio);
        }
    } else {
        estat_client->estat = NOT_SUBSCRIBED;
        printf("INFO => El client passa a estat NOT_SUBSCRIBED\n");
        subscripcio(estat_client,configuracio);
    }
}

int comprova_dades(Estat* estat_client, Configuracio* configuracio, Socket_client* socket_client, PDU* pdu) {
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

void asynchronous_read(Estat* estat_client, Socket_client* socket_client, Configuracio* configuration,
        PDU* pdu, fd_set* read_set, int result) {
    if (result > 0) {
        if (FD_ISSET(socket_client->fd, read_set)) {
            rep_resposta(estat_client, configuration, socket_client, pdu);
        }
    } else if (result < 0) {
        fprintf(stderr, "SEVERE => Error al select() \n");
    }
}