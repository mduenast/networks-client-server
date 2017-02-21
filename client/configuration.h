#ifndef CONFIGURATION_H
#define CONFIGURATION_H
typedef struct element{
    char codi_element[7 + 1];
}Element;

typedef struct configuration{
    char name[8 + 1];
    char situation[12 + 1];
    Element elements[10];
    char mac[12 + 1];
    int local_tcp;
    char server[12 + 1];
    int srv_udp;
}Configuration;
#endif /* CONFIGURATION_H */

