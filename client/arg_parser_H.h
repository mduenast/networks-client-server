#ifndef ARG_PARSER_H_H
#define ARG_PARSER_H_H

#include "configuration.h"
#include "arguments.h"
int parse_args(int argc, char** argv, Configuration* configuration, Arguments* arguments);
int set_opcions_condiguracio(char opcio_configuracio[100],Configuration* configuracio);
char* get_sub_str(char* str,int inici,int fi);
#endif /* ARG_PARSER_H_H */

