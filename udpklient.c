#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>

#define BUFFER_SIZE 30000  // Rozmiar bufora ustawiony na 30000 bajtów
#define ATTEMPTS 100       // Liczba powtórzeń

char buf[BUFFER_SIZE];    // Bufor na dane

int main(int argc, char **argv)
{
    printf("Client running...\n");
    struct sockaddr_in endpoint;
    int sdsocket, addrlen, i;
    struct timeval time_b, time_e;

    if (argc < 3) {
        printf("podaj nazwe hosta i numer portu jako parametry\n");
        return 1;
    }


    // Konfiguracja adresu serwera
    endpoint.sin_family = AF_INET;
    endpoint.sin_port = htons(atoi(argv[2]));
    endpoint.sin_addr.s_addr = inet_addr(argv[1]);
    addrlen = sizeof(struct sockaddr_in);

    // Pomiar czasu - początek
    gettimeofday(&time_b, NULL);

    for (i = 0; i < ATTEMPTS; i++) {
        // Utworzenie gniazda UDP
        sdsocket = socket(AF_INET, SOCK_DGRAM, 0);
        if (sdsocket < 0) {
            perror("socket() nie powiodl sie");
            return 1;
        }

        // Wysyłanie danych do serwera
        sendto(sdsocket, buf, BUFFER_SIZE, 0, 
               (struct sockaddr*) &endpoint, addrlen);

        // Odbiór danych z serwera
        recvfrom(sdsocket, buf, BUFFER_SIZE, 0, 
                 (struct sockaddr*) &endpoint, &addrlen);

        // Zamknięcie gniazda
        close(sdsocket);
    }

    // Pomiar czasu - koniec
    gettimeofday(&time_e, NULL);

    // Obliczenie i wyświetlenie średniego czasu
    printf("czas: %.6f s\n",
        (((double) (time_e.tv_sec - time_b.tv_sec) * 1000000) +
        ((double) (time_e.tv_usec - time_b.tv_usec)))
        / (1000000.0 * ATTEMPTS));
    return 0;
}
