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

#ifndef MANTENIR_COMUNICACIO_H
#define MANTENIR_COMUNICACIO_H

/*
 * PDU fase mantenir comunicacio
 */
typedef struct pdu{
    unsigned char tipus_paquet;
    char mac[13];
    char numero_aleatori[9];
    char dades[80];
}PDU;

#define HELLO (int) 0x10
#define HELLO_REJ (int) 0x11

#define V (int) 2

#endif /* MANTENIR_COMUNICACIO_H */

