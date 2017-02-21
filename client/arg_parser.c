
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "arguments.h"
#include "configuration.h"
#include "arg_parser_H.h"

int parse_args(int argc, char** argv, Configuration* configuration, Arguments* arguments) {
    int i;
    for (i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-c") == 0) {
            strcpy(arguments->arxiu_configuracio, argv[i + 1]);
        }
    }
    FILE* arxiu_configuracio = fopen(arguments->arxiu_configuracio, "r");
    if (arxiu_configuracio == NULL) {
        perror("Error al obrir el arxiu de configuracio");
    } else {
        char opcio_configuracio[100];
        while (fgets(opcio_configuracio, 100, arxiu_configuracio) != NULL) {
            opcio_configuracio[strlen(opcio_configuracio) - 1] = '\0';
            set_opcions_condiguracio(opcio_configuracio,configuration);
        }
        fclose(arxiu_configuracio);
    }
    return 0;
}

char* get_sub_str(char* str,int inici,int fi){
    int i,j=0;
    char* sub_str = (char*) malloc(sizeof(char) *(fi - inici + 1));
    for(i=inici;i <= fi; i++){
        sub_str[j] = str[i];
        j++;
    }
    sub_str[j] = '\0';
    return sub_str;
}

int set_opcions_condiguracio(char opcio_configuracio[100],Configuration* configuracio) {
    if (strstr(opcio_configuracio, "Name") != NULL) {
        printf("%s\n",get_sub_str(opcio_configuracio,7,strlen(opcio_configuracio)));
    } else if (strstr(opcio_configuracio, "Situation") != NULL) {
        printf("%s\n",get_sub_str(opcio_configuracio,12,strlen(opcio_configuracio)));
    } else if (strstr(opcio_configuracio, "Elements") != NULL) {
        printf("%s\n",get_sub_str(opcio_configuracio,11,strlen(opcio_configuracio)));
    } else if (strstr(opcio_configuracio, "MAC") != NULL) {
        printf("%s\n",get_sub_str(opcio_configuracio,6,strlen(opcio_configuracio)));
    } else if (strstr(opcio_configuracio, "Local-TCP") != NULL) {
        printf("%s\n",get_sub_str(opcio_configuracio,12,strlen(opcio_configuracio)));
    } else if (strstr(opcio_configuracio, "Server") != NULL) {
        printf("%s\n",get_sub_str(opcio_configuracio,9,strlen(opcio_configuracio)));
    } else if (strstr(opcio_configuracio, "Srv-UDP") != NULL) {
        printf("%s\n",get_sub_str(opcio_configuracio,10,strlen(opcio_configuracio)));
    }
    return 0;
}
