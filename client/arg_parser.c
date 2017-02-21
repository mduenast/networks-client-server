
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "arguments.h"
#include "configuration.h"
#include "arg_parser_H.h"

int parse_args(int argc, char** argv, Configuration* configuration, Arguments* arguments) {
    int i;
    strcpy(arguments->arxiu_configuracio, "client.cfg");
    for (i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-c") == 0) {
            strcpy(arguments->arxiu_configuracio, argv[i + 1]);
        }
    }
    FILE* arxiu_configuracio = fopen(arguments->arxiu_configuracio, "r");
    if (arxiu_configuracio == NULL) {
        perror("Error al obrir el arxiu de configuracio");
        return -1;
    } else {
        char opcio_configuracio[100];
        while (fgets(opcio_configuracio, 100, arxiu_configuracio) != NULL) {
            opcio_configuracio[strlen(opcio_configuracio) - 1] = '\0';
            set_opcions_condiguracio(opcio_configuracio, configuration);
        }
        fclose(arxiu_configuracio);
    }
    return 0;
}

char* get_sub_str(char* str, int inici, int fi) {
    int i, j;
    char* sub_str = (char*) malloc(sizeof (char) *(fi - inici + 1));
    for (i = inici, j = 0; i <= fi; i++, j++) {
        sub_str[j] = str[i];
    }
    sub_str[j] = '\0';
    return sub_str;
}

int get_elements(char* elements, Configuration* configuration) {
    char* token;
    token = strtok(elements, ";");
    int i = 0;
    while (token != NULL) {
        strcpy(configuration->elements[i].codi_element,token);
        printf("%s\n",configuration->elements[i].codi_element);
        token = strtok(NULL, ";");
        i++;
    }
    return 0;
}

int set_opcions_condiguracio(char opcio_configuracio[100], Configuration* configuracio) {
    if (strstr(opcio_configuracio, "Name") != NULL) {
        strcpy(configuracio->name, get_sub_str(opcio_configuracio, 7, strlen(opcio_configuracio)));
        printf("%s\n", configuracio->name);
    } else if (strstr(opcio_configuracio, "Situation") != NULL) {
        strcpy(configuracio->situation, get_sub_str(opcio_configuracio, 12, strlen(opcio_configuracio)));
        printf("%s\n", configuracio->situation);
    } else if (strstr(opcio_configuracio, "Elements") != NULL) {
        get_elements(get_sub_str(opcio_configuracio,11,strlen(opcio_configuracio)),configuracio);
    } else if (strstr(opcio_configuracio, "MAC") != NULL) {
        strcpy(configuracio->mac, get_sub_str(opcio_configuracio, 6, strlen(opcio_configuracio)));
        printf("%s\n", configuracio->mac);
    } else if (strstr(opcio_configuracio, "Local-TCP") != NULL) {
        configuracio->local_tcp = atoi(get_sub_str(opcio_configuracio, 12, strlen(opcio_configuracio)));
        printf("%i\n", configuracio->local_tcp);
    } else if (strstr(opcio_configuracio, "Server") != NULL) {
        strcpy(configuracio->server, get_sub_str(opcio_configuracio, 9, strlen(opcio_configuracio)));
        printf("%s\n", configuracio->server);
    } else if (strstr(opcio_configuracio, "Srv-UDP") != NULL) {
        configuracio->srv_udp = atoi(get_sub_str(opcio_configuracio, 10, strlen(opcio_configuracio)));
        printf("%i\n", configuracio->srv_udp);
    } else {
        return -1;
    }
    return 0;
}


