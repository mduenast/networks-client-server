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

#ifndef ENVIAR_DADES_H
#define ENVIAR_DADES_H

#include "arguments.h"
#include "estat_client.h"
#include "socket_client.h"
#include <pthread.h>


typedef struct pdu_enviar_dades{
    unsigned char tipus_paquet;
    char mac[13];
    char numero_aleatori[9];
    char dispositiu[8];
    char valor[7];
    char info[80];
}PDU_Enviar_dades;

#define SEND_DATA (int) 0x20
#define SET_DATA (int) 0x21
#define GET_DATA (int) 0x22
#define DATA_ACK (int) 0x23
#define DATA_NACK (int) 0x24
#define DATA_REJ (int) 0x25

#define W (int) 3

typedef struct parametres{
    Configuracio* configuracio;
    Estat* estat_client;
}Parametres;

int inicia_socket_tcp_rebre(Configuracio* configuracio, Socket_client_rebre_dades* socket_client);
void* rebre_dades(void* params);
int comprova_dades_enviar_dades(Estat* estat_client,
        Configuracio* configuracio, Socket_client_enviar_dades* socket_client_enviar_dades, 
        PDU_Enviar_dades* pdu);
void prepara_pdu_enviar_dades(PDU_Enviar_dades* pdu,unsigned char tipus_paquet,
        char* mac,char* numero_aleatori,char* dispositiu,char* valor,char* info);
int inicia_socket_tcp_send(Configuracio* configuracio, Socket_client_enviar_dades* socket);
int envia_dades(char* commanda,Configuracio* configuracio,Estat* estat_client);
void quit();
void stat(Configuracio* configuracio);
void set(char* commanda,Configuracio* configuracio);
void* espera_comandes(void* params);
int comandes(Estat* estat_client, Configuracio* configuracio);


#endif /* ENVIAR_DADES_H */

