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

int subscripcio(Estat* estat_client, Configuracio* configuracio) {
    estat_client->estat = DISCONNECTED;
    printf("El client passa al estat DISCONNECTED\n");
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
    envia(socket_client, pdu);
    estat_client->estat = WAIT_ACK_SUBS;
    printf("El client passa al estat WAIT_ACK_SUBS\n");
    //int i;
    //for(i=1;i<N;i++){
        comprova_resposta(socket_client,pdu);
        //envia(socket_client, pdu);
    //}
    return 0;
}

int start_socket(Socket_client* socket_client, Configuracio* configuracio) {
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

void prepara_pdu(PDU* pdu, unsigned char tipus_paquet, Configuracio* configuracio, char* numero_aleatori, char* dades) {
    pdu->tipus_paquet = tipus_paquet;
    strcpy(pdu->mac, configuracio->mac);
    strcpy(pdu->numero_aleatori, numero_aleatori);
    strcpy(pdu->dades, dades);
}

void envia(Socket_client* socket_client, PDU* pdu) {
    int bytes = sendto(socket_client->fd, pdu, sizeof (PDU), 0,
            (struct sockaddr*) &(socket_client->server), sizeof (struct sockaddr));
    if (bytes == -1) {
        fprintf(stderr, "sendto() error\n");
    }
    printf("Envia => tipus paquet : %c , mac : %s , numero aleatori : %s , dades : %s\n",
            pdu->tipus_paquet, pdu->mac, pdu->numero_aleatori, pdu->dades);
}

void comprova_resposta(Socket_client* socket_client,PDU* pdu){
    socklen_t socklen = sizeof(struct sockaddr);
    int bytes = recvfrom(socket_client->fd,pdu,sizeof(PDU),0,(struct sockaddr*)&(socket_client->server),&socklen);
    if (bytes == -1) {
        fprintf(stderr, "recvfrom() error\n");
    }
    printf("Rep => tipus paquet : %c , mac : %s , numero aleatori : %s , dades : %s\n",
            pdu->tipus_paquet, pdu->mac, pdu->numero_aleatori, pdu->dades);
}