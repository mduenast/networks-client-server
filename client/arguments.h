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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef ARGUMENTS_H
#define ARGUMENTS_H 

typedef struct arguments {
    char arxiu_configuracio[20];
} Arguments;

typedef struct element {
    char codi[7 + 1];
} Element;

typedef struct configuracio {
    char name[8 + 1];
    char situation[20];
    Element elements[10];
    char mac[12 + 1];
    int local_tcp;
    char server[20];
    int srv_udp;
} Configuracio;

void init_arguments(Arguments* arguments);
int parse_args(int argc, char** argv, Arguments* arguments, Configuracio* configuracio);
int read_configuration(Configuracio* configuracio, Arguments* arguments);
void get_elements(char* value, Configuracio* configuracio);

#endif /* ARGUMENTS_H */

