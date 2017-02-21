
#include <stdio.h>
#include <string.h>

#include "arguments.h"
#include "configuration.h"


int parse_args(int argc,char** argv,Configuration* configuration,Arguments* arguments){
    int i;
    for(i=0; i<argc;i++){
        if(strcmp(argv[i],"-c") == 0){
            strcpy(arguments->arxiu_configuracio,argv[i+1]);
        }
    }
    FILE* arxiu_configuracio = fopen(arguments->arxiu_configuracio,"r");
    if (arxiu_configuracio == NULL){
        perror("Error al obrir el arxiu de configuracio");
    }
    else {
        /*if ( fgets (mystring , 100 , pFile) != NULL )
          puts (mystring);
        fclose (pFile);*/
        
    }
    return 0;
}