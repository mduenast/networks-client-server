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

void init_arguments(Arguments* arguments) {
    strcpy(arguments->arxiu_configuracio, "client.cfg");
}

void get_elements(char* value, Configuracio* configuracio) {
    char * token;
    int i = 0;
    token = strtok(value, ";");
    while (token != NULL) {
        if (i < 10) {
            strcpy(configuracio->elements[i].codi, token);
            token = strtok(NULL, ";");
        }
        i++;
    }
}

int parse_args(int argc, char** argv, Arguments* arguments, Configuracio* configuracio,Estat* estat_client) {
    int i;
    init_arguments(arguments);
    for (i = 0; i < argc; i++) {
        if (strcmp("-c", argv[i]) == 0) {
            strcpy(arguments->arxiu_configuracio, argv[i + 1]);
        }
        else if(strcmp("-d",argv[i]) == 0){
            estat_client->debug = 1;
        }
        else if(strcmp("\n",argv[i]) == 0 || strcmp("./client",argv[i]) == 0){
            
        }
        else{
            printf("Commanda no reconeguda\n");
        }
    }
    read_configuration(configuracio, arguments);
    return 0;
}

int read_configuration(Configuracio* configuracio, Arguments* arguments) {
    FILE* arxiu_configuracio = fopen(arguments->arxiu_configuracio, "r");
    if (arxiu_configuracio == NULL) {
        perror("Error obrint el fitxer\n");
    }
    char key[50];
    char value[50];
    while (fscanf(arxiu_configuracio, "%s = %s\n", key, value) != EOF) {
        if (strcmp("Name", key) == 0) {
            strcpy(configuracio->name, value);
        } else if (strcmp("Situation", key) == 0) {
            strcpy(configuracio->situation, value);
        } else if (strcmp("Elements", key) == 0) {
            get_elements(value, configuracio);
        } else if (strcmp("MAC", key) == 0) {
            strcpy(configuracio->mac, value);
        } else if (strcmp("Local-TCP", key) == 0) {
            configuracio->local_tcp = atoi(value);
        } else if (strcmp("Server", key) == 0) {
            strcpy(configuracio->server, value);
        } else if (strcmp("Srv-UDP", key) == 0) {
            configuracio->srv_udp = atoi(value);
        }
    }
    return 0;
}