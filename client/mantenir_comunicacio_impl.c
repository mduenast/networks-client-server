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
#include "subscripcio.h"
#include "enviar_rebre_dades_comandes.h"
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <pthread.h>
#include <signal.h>

void alarm_handle(int sig) {
    signal(SIGALRM, alarm_handle);
}

int start_socket_mantenir_comunicacio(Socket_client* socket_client, Configuracio* configuracio) {
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

int mantenir_comunicacio(Estat* estat_client, Configuracio* configuracio) {
    Socket_client* socket_client = (Socket_client*) malloc(sizeof (Socket_client));
    if (start_socket_mantenir_comunicacio(socket_client, configuracio) == -1) {
        return -1;
    }

    // prepara el paquet
    PDU_comunicacio* pdu = (PDU_comunicacio*) malloc(sizeof (PDU_comunicacio));
    char dades[80];
    sprintf(dades, "%s,%s", configuracio->name, configuracio->situation);
    prepara_pdu_mantenir_comunicacio(pdu, HELLO, configuracio, configuracio->dades_servidor.numero_aleatori, dades);

    // envia el paquet
    envia_mantenir_comunicacio(estat_client, socket_client, pdu);

    fd_set read_set;
    FD_ZERO(&read_set);
    FD_SET(socket_client->fd, &read_set);

    struct timeval time_out;
    time_out.tv_sec = V * R;
    time_out.tv_usec = 0;

    signal(SIGALRM, alarm_handle);
    alarm(V);
    pause();

    int result = select(socket_client->fd + 1, &read_set, NULL, NULL, &time_out);
    if (result == 0) {
        if (estat_client->debug == 1) {
            estat_client->estat = NOT_SUBSCRIBED;
            printf("DEBUG => S'ha perdut la connexio\n");
            return 0;
        }
    }
    asynchronous_read_mantenir_comunicacio(estat_client, socket_client, configuracio, pdu, &read_set, result);
    if (estat_client->estat == NOT_SUBSCRIBED) {
        return 0;
    }
    int perduts = 0;

    int rep_comandes = 0;
    pthread_t thread_comandes = 0;
    pthread_t thread_rebre_dades = 0;

    time_out.tv_sec = 0;

    while (1) {
        // esperem commandes
        if (estat_client->estat == SEND_HELLO && rep_comandes == 0) {
            rep_comandes++;
            Parametres* params = (Parametres*)
                    malloc(sizeof (Parametres));
            params->estat_client = estat_client;
            params->configuracio = configuracio;
            if (pthread_create(&thread_comandes, NULL, (void* (*)(void*)) espera_comandes, (void*) (params))) {
                fprintf(stderr, "SEVERE => Error creant thread\n");
                return -1;
            }
            if (estat_client->debug == 1) {
                printf("DEBUG => Comença el fil de rebre commandes\n");
            }
            if (pthread_create(&thread_rebre_dades, NULL, (void* (*)(void*)) rebre_dades, (void*) (params))) {
                fprintf(stderr, "SEVERE => Error creant thread\n");
                return -1;
            }
            if (estat_client->debug == 1) {
                printf("DEBUG => Comença el fil de rebre dades del servidor\n");
            }
        }



        sprintf(dades, "%s,%s", configuracio->name, configuracio->situation);
        prepara_pdu_mantenir_comunicacio(pdu, HELLO, configuracio, configuracio->dades_servidor.numero_aleatori, dades);
        envia_mantenir_comunicacio(estat_client, socket_client, pdu);

        alarm(V);
        pause();

        FD_ZERO(&read_set);
        FD_SET(socket_client->fd, &read_set);
        int result = select(socket_client->fd + 1, &read_set, NULL, NULL, &time_out);
        if (result == 0) {
            perduts++;
        } else {
            perduts = 0;
        }
        if (perduts == S) {
            estat_client->estat = NOT_SUBSCRIBED;
            printf("INFO => El client passa a estat NOT_SUBSCRIBED\n");
            close(socket_client->fd);
            break;
        }
        asynchronous_read_mantenir_comunicacio(estat_client, socket_client, configuracio, pdu, &read_set, result);
        if (estat_client->estat == NOT_SUBSCRIBED) {
            break;
        }
    }
    if (pthread_join(thread_comandes, NULL)) {
        fprintf(stderr, "SEVERE => Error join thread\n");
        return -1;
    }
    if (estat_client->debug == 1) {
        printf("DEBUG => Finalitza el fil de espera commandes\n");
    }
    if (pthread_join(thread_rebre_dades, NULL)) {
        fprintf(stderr, "SEVERE => Error join thread\n");
        return -1;
    }
    if (estat_client->debug == 1) {
        printf("DEBUG => Finalitza el fil de rebre dades del servidor\n");
    }
    return 0;
}

void envia_mantenir_comunicacio(Estat* estat_client, Socket_client* socket_client,
        PDU_comunicacio* pdu) {
    int bytes = sendto(socket_client->fd, pdu, sizeof (PDU_comunicacio), 0,
            (struct sockaddr*) &(socket_client->server), sizeof (struct sockaddr));
    if (estat_client->debug == 1) {
        printf("DEBUG => Envia { tipus paquet : %c , mac : %s , numero aleatori : %s , dades : %s }\n",
                pdu->tipus_paquet, pdu->mac, pdu->numero_aleatori, pdu->dades);
    }
    if (bytes == -1) {

        fprintf(stderr, "SEVERE => sendto() error\n");
    }
}

void asynchronous_read_mantenir_comunicacio(Estat* estat_client,
        Socket_client* socket_client, Configuracio* configuration,
        PDU_comunicacio* pdu, fd_set* read_set, int result) {
    if (result > 0) {
        if (FD_ISSET(socket_client->fd, read_set)) {
            rep_resposta_mantenir_comunicacio(estat_client, configuration, socket_client, pdu);
        }
    } else if (result < 0) {

        fprintf(stderr, "SEVERE => Error al select() \n");
    }
}

void rep_resposta_mantenir_comunicacio(Estat* estat_client,
        Configuracio* configuracio, Socket_client* socket_client, PDU_comunicacio* pdu) {
    socklen_t socklen = sizeof (struct sockaddr);
    int bytes = recvfrom(socket_client->fd, pdu, sizeof (PDU_comunicacio),
            0, (struct sockaddr*) &(socket_client->server), &socklen);
    if (estat_client->debug == 1) {
        printf("DEBUG => Rep { tipus paquet : %c , mac : %s , numero aleatori : %s , dades : %s }\n",
                pdu->tipus_paquet, pdu->mac, pdu->numero_aleatori, pdu->dades);
    }
    if (bytes == -1) {

        fprintf(stderr, "SEVERE => recvfrom() error\n");
    }
    comprova_resposta_mantenir_comunicacio(estat_client, configuracio, socket_client, pdu);
}

void comprova_resposta_mantenir_comunicacio(Estat* estat_client,
        Configuracio* configuracio, Socket_client* socket_client, PDU_comunicacio* pdu) {
    if (pdu->tipus_paquet == HELLO) {
        char dades[80];
        sprintf(dades, "%s,%s", configuracio->name, configuracio->situation);
        if (comprova_dades_mantenir_comunicacio(estat_client, configuracio, socket_client, pdu) == 0 &&
                strcmp(pdu->dades, dades) == 0) {
            if (estat_client->estat != SEND_HELLO) {
                printf("INFO => El client passa a estat SEND_HELLO\n");
            }
            estat_client->estat = SEND_HELLO;
        } else {
            estat_client->estat = NOT_SUBSCRIBED;
            if (estat_client->debug == 1) {
                printf("DEBUG => El client passa a estat NOT_SUBSCRIBED\n");
            }
            subscripcio(estat_client,configuracio);
        }
    } else {
        subscripcio(estat_client,configuracio);
    }
}