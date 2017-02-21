#ifndef ESTATS_FASE_REGISTRE_H
#define ESTATS_FASE_REGISTRE_H
const int DISCONNECTED = 0XA0;
const int NOT_SUBSCRIBED = 0XA1;
const int WAIT_ACK_SUBS = 0XA2;
const int WAIT_ACK_INFO = 0XA3;
const int SUBSCRIBED = 0XA4;
const int SEND_HELLO = 0XA5;
const int WAIT_INFO = 0XA6;

typedef struct estat_client{
    int estat;
}Estat_client;
#endif /* ESTATS_FASE_REGISTRE_H */

