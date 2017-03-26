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

#include "enviar_rebre_dades_comandes.h"
#include "estat_client.h"
#include "subscripcio.h"
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <pthread.h>

#define BACKLOG (int) 1

void* espera_comandes(void* params) {
    Parametres* parametres = (Parametres*) params;

    if (comandes(parametres->estat_client, parametres->configuracio) == -1) {
        fprintf(stderr, "Hi ha hagut un error inesperat\n");
        return NULL;
    }

    return NULL;
}

void* rebre_dades(void* params) {
    Parametres* parametres = (Parametres*) params;
    int sin_size;
    Socket_client_rebre_dades* socket_client = (Socket_client_rebre_dades*)
            malloc(sizeof (Socket_client_rebre_dades));
    if (inicia_socket_tcp_rebre(parametres->configuracio, socket_client) == -1) {
        fprintf(stderr, "SEVERE => Error al obrir el socket\n");
        return NULL;
    }
    socket_client->fd_client = -1;
    fd_set read_set;
    FD_ZERO(&read_set);
    FD_SET(socket_client->fd_server, &read_set);
    struct timeval time_out;
    time_out.tv_sec = 0;
    time_out.tv_usec = 0;
    while (parametres->estat_client->estat == SEND_HELLO) {

        FD_ZERO(&read_set);
        FD_SET(socket_client->fd_server, &read_set);

        sin_size = sizeof (struct sockaddr_in);
        int result = select(socket_client->fd_server + 1, &read_set, NULL, NULL, &time_out);
        if (result > 0) {
            if (FD_ISSET(socket_client->fd_server, &read_set)) {
                /* A continuación la llamada a accept() */

                if ((socket_client->fd_client = accept(socket_client->fd_server, (struct sockaddr *) &(socket_client->client),
                        &sin_size)) == -1) {
                    printf("SEVERE => error en accept()\n");
                    return NULL;
                }
                printf("INFO => S'ha obtingut una connexio desde => %s\n",
                        inet_ntoa((socket_client->client).sin_addr));
                /* que mostrará la IP del cliente */
                int bytes = 0;
                PDU_Rebre_dades* pdu = (PDU_Rebre_dades*) malloc(sizeof (PDU_Rebre_dades));

                time_out.tv_sec = W;
                FD_ZERO(&read_set);
                FD_SET(socket_client->fd_client, &read_set);
                result = select(socket_client->fd_client + 1, &read_set, NULL, NULL, &time_out);
                if (result > 0) {
                    if (FD_ISSET(socket_client->fd_client, &read_set)) {
                        if ((bytes = recv(socket_client->fd_client, pdu, sizeof (PDU_Rebre_dades), 0)) == -1) {
                            printf("SEVERE => Error al recv()\n");
                            return NULL;
                        }
                        if (parametres->estat_client->debug == 1) {
                            printf("DEBUG => Rebut { tipus paquet = %u , mac = %s , numero aleatori = %s ,"
                                    " dispositiu = %s , valor = %s , info = %s }\n", pdu->tipus_paquet, pdu->mac, pdu->numero_aleatori, pdu->dispositiu,
                                    pdu->valor, pdu->info);
                        }
                        if (comprova_dades_rebre_dades(parametres->estat_client, parametres->configuracio, socket_client, pdu) == 0) {
                            if (pdu->tipus_paquet == SET_DATA) {
                                int i;
                                for (i = 0; i<sizeof (parametres->configuracio->elements) / sizeof (Element); i++) {
                                    if (strcmp(pdu->dispositiu, parametres->configuracio->elements[i].codi) == 0) {
                                        strcpy(parametres->configuracio->elements[i].valor, pdu->valor);
                                        break;
                                    }
                                }
                                PDU_Rebre_dades* pdu_resposta = (PDU_Rebre_dades*) malloc(sizeof (PDU_Rebre_dades));
                                prepara_pdu_rebre_dades(pdu_resposta, DATA_ACK, parametres->configuracio->mac,
                                        parametres->configuracio->dades_servidor.numero_aleatori, pdu->dispositiu, pdu->valor, "Canvi aplicat");
                                if ((bytes = send(socket_client->fd_client, pdu_resposta, sizeof (PDU_Rebre_dades), 0)) == -1) {
                                    printf("SEVERE => Error al send()\n");
                                }
                                if (parametres->estat_client->debug == 1) {
                                    printf("DEBUG => Enviat { tipus paquet = %u , mac = %s , numero aleatori = %s ,"
                                            " dispositiu = %s , valor = %s , info = %s }\n", pdu_resposta->tipus_paquet, pdu_resposta->mac,
                                            pdu_resposta->numero_aleatori, pdu_resposta->dispositiu, pdu_resposta->valor, pdu_resposta->info);
                                }
                            } else if (pdu->tipus_paquet == GET_DATA) {
                                int i;
                                for (i = 0; i<sizeof (parametres->configuracio->elements) / sizeof (Element); i++) {
                                    if (strcmp(pdu->dispositiu, parametres->configuracio->elements[i].codi) == 0) {
                                        strcpy(pdu->valor, parametres->configuracio->elements[i].valor);
                                        break;
                                    }
                                }
                                PDU_Rebre_dades* pdu_resposta = (PDU_Rebre_dades*) malloc(sizeof (PDU_Rebre_dades));
                                prepara_pdu_rebre_dades(pdu_resposta, DATA_ACK, parametres->configuracio->mac,
                                        parametres->configuracio->dades_servidor.numero_aleatori, pdu->dispositiu, pdu->valor, "Dades enviades");
                                if ((bytes = send(socket_client->fd_client, pdu_resposta, sizeof (PDU_Rebre_dades), 0)) == -1) {
                                    printf("SEVERE => Error al send()\n");
                                }
                                if (parametres->estat_client->debug == 1) {
                                    printf("DEBUG => Enviat { tipus paquet = %u , mac = %s , numero aleatori = %s ,"
                                            " dispositiu = %s , valor = %s , info = %s }\n", pdu_resposta->tipus_paquet, pdu_resposta->mac,
                                            pdu_resposta->numero_aleatori, pdu_resposta->dispositiu, pdu_resposta->valor, pdu_resposta->info);
                                }
                            }
                        } else {
                            printf("NO VALIDAT\n");
                        }
                    }
                } else if (result < 0) {
                    fprintf(stderr, "SEVERE => Error al select()\n");
                }
                time_out.tv_sec = 0;
            }
        } else if (result < 0) {
            fprintf(stderr, "SEVERE => Error al select()\n");
        }
    }
    close(socket_client->fd_server);
    if (socket_client->fd_client != -1) {
        close(socket_client->fd_client); /* cierra fd2 */
    }
    return NULL;
}

void quit() {
    printf("Sortint ...\n");
    kill(getpid(), SIGINT);
}

void stat(Configuracio* configuracio) {
    printf("+--------------+\n");
    printf("|     stat     |\n");
    printf("+--------------+\n");
    printf("* MAC : %s\n", configuracio->mac);
    printf("* Nom : %s\n", configuracio->name);
    printf("* Situacio : %s\n", configuracio->situation);
    int i;
    for (i = 0; i < 10; i++) {
        printf("* Element %i : { codi = %s , valor = %s }\n", i,
                strlen(configuracio->elements[i].codi) == 0 ? "No assignat" : configuracio->elements[i].codi,
                configuracio->elements[i].valor);
    }
    printf("*********************************************\n");
}

void set(char* commanda, Configuracio* configuracio) {
    char dispositiu[20];
    char valor[20];
    sscanf(commanda, "set %s %s\n", dispositiu, valor);
    int i;
    for (i = 0; i<sizeof (configuracio->elements) / sizeof (Element); i++) {
        if (strcmp(dispositiu, configuracio->elements[i].codi) == 0) {
            strcpy(configuracio->elements[i].valor, valor);
            break;
        }
    }
}

int envia_dades(char* commanda, Configuracio* configuracio, Estat* estat_client) {
    char nom_dispositiu[20];
    sscanf(commanda, "send %s\n", nom_dispositiu);
    Socket_client_enviar_dades* socket = (Socket_client_enviar_dades*)
            malloc(sizeof (Socket_client_enviar_dades));
    if (inicia_socket_tcp_send(configuracio, socket) == -1) {
        fprintf(stderr, "SEVERE => Error al obrir el socket\n");
        return -1;
    }
    int i;
    for (i = 0; i < (sizeof (configuracio->elements)) / (sizeof (Element)); i++) {
        if (strcmp(configuracio->elements[i].codi, nom_dispositiu) == 0) {
            PDU_Enviar_dades* pdu = (PDU_Enviar_dades*) malloc(sizeof (PDU_Enviar_dades));
            prepara_pdu_enviar_dades(pdu, SEND_DATA, configuracio->mac,
                    configuracio->dades_servidor.numero_aleatori, nom_dispositiu,
                    configuracio->elements[i].valor, "");
            if (estat_client->debug == 1) {
                printf("DEBUG => Enviant { Dispositiu = %s , valor = %s }\n",
                        nom_dispositiu, configuracio->elements[i].valor);
            }
            int bytes = send(socket->fd, pdu, sizeof (PDU_Enviar_dades), 0);
            if (bytes == -1) {
                fprintf(stderr, "SEVERE => send()\n");
                return -1;
            }
            fd_set read_set;
            FD_ZERO(&read_set);
            FD_SET(socket->fd, &read_set);
            struct timeval time_out;
            time_out.tv_sec = W;
            time_out.tv_usec = 0;

            int result = select(socket->fd + 1, &read_set, NULL, NULL, &time_out);
            if (result > 0) {
                if (FD_ISSET(socket->fd, &read_set)) {
                    bytes = recv(socket->fd, pdu, sizeof (PDU_Enviar_dades), 0);
                    if (estat_client->debug == 1) {
                        printf("Rebut {tipus paquet = %u , mac = %s , numero aleatori = %s , "
                                "dispositiu = %s , valor = %s , info = %s }\n",
                                pdu->tipus_paquet, pdu->numero_aleatori, pdu->numero_aleatori,
                                pdu->dispositiu, pdu->valor, pdu->info);
                    }
                    if (bytes == -1) {
                        fprintf(stderr, "SEVERE => recvfrom() error\n");
                    }
                    if (comprova_dades_enviar_dades(estat_client, configuracio, socket, pdu) == 0) {
                        if (pdu->tipus_paquet == DATA_ACK) {
                            printf("INFO => Les dades han estat acceptades\n");
                        } else if (pdu->tipus_paquet == DATA_NACK) {
                            printf("INFO => Les dades no han estat acceptades\n");
                        } else if (pdu->tipus_paquet == DATA_REJ) {
                            printf("INFO => Les dades han sigut rebutjades\n");
                            estat_client->estat = NOT_SUBSCRIBED;
                            printf("INFO => El client passa a estat NOT_SUBSCRIBED\n");
                            return -1;
                        }
                    } else {
                        estat_client->estat = NOT_SUBSCRIBED;
                        printf("INFO => El client passa a estat NOT_SUBSCRIBED\n");
                        return -1;
                    }
                }
            } else if (result < 0) {
                fprintf(stderr, "SEVERE => Error al select() \n");
            }
        }
    }
    close(socket->fd);
    return 0;
}

int comandes(Estat* estat_client, Configuracio * configuracio) {
    fd_set read_set;
    FD_ZERO(&read_set);
    FD_SET(0, &read_set);
    struct timeval time_out;
    time_out.tv_sec = 0;
    time_out.tv_usec = 0;
    int result = 0;

    char commanda [100];
    while (estat_client->estat == SEND_HELLO) {
        FD_ZERO(&read_set);
        FD_SET(0, &read_set);
        result = select(1, &read_set, NULL, NULL, &time_out);
        if (result > 0) {
            if (FD_ISSET(0, &read_set)) {
                if (fgets(commanda, 100, stdin) != NULL) {
                    if (strcmp("quit\n", commanda) == 0) {
                        quit();
                    } else if (strcmp("stat\n", commanda) == 0) {
                        stat(configuracio);
                    } else if (strncmp("set", commanda, strlen("set")) == 0) {
                        set(commanda, configuracio);
                    } else if (strncmp("send", commanda, strlen("send")) == 0) {
                        if (envia_dades(commanda, configuracio, estat_client) == -1) {
                            return -1;
                        }
                    } else if (strcmp("\n", commanda) == 0) {

                    } else {
                        fprintf(stderr, "No es reconeix la commanda\n");
                    }
                }
            }
        } else if (result < 0) {
            fprintf(stderr, "SEVERE => Error al select() \n");
        }
    }
    return 0;
}

int inicia_socket_tcp_send(Configuracio* configuracio, Socket_client_enviar_dades * socket_enviar) {
    if ((socket_enviar->he = gethostbyname(configuracio->server)) == NULL) {
        /* llamada a gethostbyname() */
        printf("SEVERE => gethostbyname()\n");
        return -1;
    }

    if ((socket_enviar->fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        /* llamada a socket() */
        printf("SEVERE => socket()\n");
        return -1;
    }
    socket_enviar->server.sin_family = AF_INET;
    socket_enviar->server.sin_port = htons(configuracio->tcp_enviar_rebre_dades);
    socket_enviar->server.sin_addr = *((struct in_addr *) socket_enviar->he->h_addr_list[0]);
    memset(&(socket_enviar->server.sin_zero), 0, 8);

    if (connect(socket_enviar->fd, (struct sockaddr *) &(socket_enviar->server),
            sizeof (struct sockaddr)) == -1) {
        /* llamada a connect() */
        printf("SEVERE => connect()\n");
        return -1;
    }
    return 0;
}

void prepara_pdu_enviar_dades(PDU_Enviar_dades* pdu, unsigned char tipus_paquet,
        char* mac, char* numero_aleatori, char* dispositiu, char* valor, char* info) {
    pdu->tipus_paquet = tipus_paquet;
    strcpy(pdu->mac, mac);
    strcpy(pdu->numero_aleatori, numero_aleatori);
    strcpy(pdu->dispositiu, dispositiu);
    strcpy(pdu->valor, valor);
    strcpy(pdu->info, info);
}

void prepara_pdu_rebre_dades(PDU_Rebre_dades* pdu, unsigned char tipus_paquet,
        char* mac, char* numero_aleatori, char* dispositiu, char* valor, char* info) {
    pdu->tipus_paquet = tipus_paquet;
    strcpy(pdu->mac, mac);
    strcpy(pdu->numero_aleatori, numero_aleatori);
    strcpy(pdu->dispositiu, dispositiu);
    strcpy(pdu->valor, valor);
    strcpy(pdu->info, info);
}

int comprova_dades_enviar_dades(Estat* estat_client,
        Configuracio* configuracio, Socket_client_enviar_dades* socket_client_enviar_dades,
        PDU_Enviar_dades * pdu) {
    if (strcmp(configuracio->dades_servidor.ip, inet_ntoa(socket_client_enviar_dades->server.sin_addr)) != 0) {
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

int comprova_dades_rebre_dades(Estat* estat_client,
        Configuracio* configuracio, Socket_client_rebre_dades* socket_client_rebre_dades,
        PDU_Rebre_dades * pdu) {
    if (strcmp(configuracio->dades_servidor.ip, inet_ntoa(socket_client_rebre_dades->client.sin_addr)) != 0) {
        printf("falla la ip");
        return -1;
    }
    if (strcmp(configuracio->dades_servidor.mac, pdu->mac) != 0) {
        printf("fall la mac");
        return -1;
    }
    if (strcmp(configuracio->dades_servidor.numero_aleatori, pdu->numero_aleatori) != 0) {
        printf("falla el numero");
        return -1;
    }
    return 0;
}

int inicia_socket_tcp_rebre(Configuracio* configuracio, Socket_client_rebre_dades* socket_client) {
    if ((socket_client->fd_server = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("SEVERE => error en socket()\n");
        return -1;
    }

    socket_client->server.sin_family = AF_INET;

    socket_client->server.sin_port = htons(configuracio->local_tcp);
    /* ¿Recuerdas a htons() de la sección "Conversiones"? =) */

    socket_client->server.sin_addr.s_addr = INADDR_ANY;
    /* INADDR_ANY coloca nuestra dirección IP automáticamente */

    memset(&(socket_client->server.sin_zero), 0, 8);
    /* A continuación la llamada a bind() */
    if (bind(socket_client->fd_server, (struct sockaddr*) &(socket_client->server),
            sizeof (struct sockaddr)) == -1) {
        printf("SEVERE => error en bind() \n");
        return -1;
    }
    /* llamada a listen() */
    if (listen(socket_client->fd_server, BACKLOG) == -1) {
        printf("SEVERE => error en listen()\n");
        return -1;
    }
    return 0;
}