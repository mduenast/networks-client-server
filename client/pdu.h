#ifndef PDU_H
#define PDU_H
typedef struct pdu{
    unsigned char tipus_paquet;
    char mac[13];
    char numero_aleatori[9];
    char dades[80];
}PDU;
#endif /* PDU_H */

