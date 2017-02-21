#include <stdio.h>
#include <stdlib.h>

#include "arg_parser_H.h"
#include "configuration.h"
#include "arguments.h"

int main(int argc, char** argv) {
    Configuration* configuration = (Configuration*) malloc(sizeof(Configuration));
    Arguments* arguments = (Arguments*) malloc(sizeof(Arguments));
    parse_args(argc,argv,configuration,arguments);
    return (EXIT_SUCCESS);
}

