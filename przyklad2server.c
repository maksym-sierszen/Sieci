#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

/* rozmiar bufora */
#define BUFFER_SIZE 10000

/* liczba powtorzen */
#define ATTEMPTS 100

char buf[BUFFER_SIZE];

/*
argv[1] - numer portu
*/
int main(int argc, char **argv)
{
    struct sockaddr_in myaddr, endpoint;
    int sdsocket, sdconnection, addrlen, received;

    if (argc < 2) {
        printf("podaj numer portu jako parametr\n");
        return 1;
    }

    sdsocket = socket(AF_INET, SOCK_STREAM, 0);
    addrlen = sizeof(struct sockaddr_in);

    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(atoi(argv[1]));
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sdsocket,(struct sockaddr*) &myaddr,addrlen) < 0) {
        printf("bind() nie powiodl sie\n");
        return 1;
    }

    if (listen(sdsocket, 10) < 0) {
        printf("listen() nie powiodl sie\n");
        return 1;
    }

    while ((sdconnection = 
            accept(sdsocket, 
                   (struct sockaddr*) &endpoint, 
                   &addrlen)) >= 0) 
    {
        received = 0;
        /* odbior moze odbywac sie w mniejszych
        segmentach */
        while (received < BUFFER_SIZE)
        {
            received += recv(sdconnection, 
                             buf+received, 
                             BUFFER_SIZE-received,
                             0);
        }
        send(sdconnection, buf, BUFFER_SIZE, 0);
        close(sdconnection);
    }

    close(sdsocket);
    return 0;
}