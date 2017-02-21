#include <stdio.h>
#include <stdlib.h>

#include "arg_parser_H.h"
#include "configuration.h"
#include "arguments.h"

int main(int argc, char** argv) {
    Configuration* configuration = (Configuration*) malloc(sizeof(Configuration));
    Arguments* arguments = (Arguments*) malloc(sizeof(Arguments));
    if(parse_args(argc,argv,configuration,arguments) == -1){
        fprintf(stderr,"Hi ha un error inesperat\n");
        return(EXIT_FAILURE);
    }
    return (EXIT_SUCCESS);
}

