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

#include <stdio.h>
#include <stdlib.h>

#include "estat_client.h"
#include "arguments.h"
#include "subscripcio.h"
#include "mantenir_comunicacio.h"

/*
 * 
 */

int main(int argc, char** argv) {
    Estat* estat_client = (Estat*) malloc(sizeof (Estat));
    Arguments* arguments = (Arguments*) malloc(sizeof (Arguments));
    Configuracio* configuracio = (Configuracio*) malloc(sizeof (Configuracio));
    estat_client->debug = 0;
    // Entren els arguments
    if (parse_args(argc, argv, arguments, configuracio,estat_client) == -1) {
        fprintf(stderr, "SEVERE => Error inesperat\n");
        return (EXIT_FAILURE);
    }
    while (1) {
        // fase de registre
        if (subscripcio(estat_client, configuracio) == -1) {
            fprintf(stderr, "SEVERE => Error inesperat\n");
            return (EXIT_FAILURE);
        }
        // fase mantenir comunicacio
        if (estat_client->estat == SUBSCRIBED) {
            if(mantenir_comunicacio(estat_client,configuracio) == -1){
                fprintf(stderr,"SEVERE => Error inesperat\n");
                return(EXIT_FAILURE);
            }
        }
    }
    return (EXIT_SUCCESS);
}

