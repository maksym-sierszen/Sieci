#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUFFER_SIZE 30000  // Rozmiar bufora ustawiony na 30000 bajtów
#define ATTEMPTS 100       // Liczba powtórzeń

char buf[BUFFER_SIZE];    // Bufor na dane

int main(int argc, char **argv)
{
    printf("Server running ...\n");
    struct sockaddr_in myaddr, clientaddr;
    int sdsocket, addrlen, received, i;

    if (argc < 2) {
        printf("podaj numer portu jako parametr\n");
        return 1;
    }

    // Utworzenie gniazda UDP
    sdsocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (sdsocket < 0) {
        perror("socket() nie powiodl sie");
        return 1;
    }

    addrlen = sizeof(struct sockaddr_in);

    // Konfiguracja adresu serwera
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(atoi(argv[1]));
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Przypisanie adresu do gniazda
    if (bind(sdsocket, (struct sockaddr*) &myaddr, addrlen) < 0) {
        perror("bind() nie powiodl sie");
        return 1;
    }

    // Główna pętla serwera
    for (i = 0; i < ATTEMPTS; i++) {
        received = recvfrom(sdsocket, buf, BUFFER_SIZE, 0, 
                            (struct sockaddr*) &clientaddr, &addrlen);
        if (received < 0) {
            perror("recvfrom() nie powiodl sie");
            continue;
        }

        // Wysyłanie danych z powrotem do klienta
        sendto(sdsocket, buf, received, 0, 
               (struct sockaddr*) &clientaddr, addrlen);
    }

    // Zamknięcie gniazda
    close(sdsocket);
    return 0;
}
