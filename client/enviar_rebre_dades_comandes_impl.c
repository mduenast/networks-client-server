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
#include <stdio.h>
#include <stdlib.h>

void* espera_comandes(void* params) {
    Parametres* parametres = (Parametres*) params;
    if (comandes(parametres->estat_client, parametres->configuracio) == -1) {
        fprintf(stderr,"Hi ha hagut un error inesperat\n");
        exit(EXIT_FAILURE);
    }
    return NULL;
}

int comandes(Estat* estat_client, Configuracio* configuracio) {
    while (estat_client->estat == SEND_HELLO) {
        char commanda [100];
        if (fgets(commanda, 100, stdin) != NULL) {
            printf("Comanda = %s \n",commanda);
        }
    }
    return 0;
}