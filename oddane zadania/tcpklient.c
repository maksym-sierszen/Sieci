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

#define BUFFER_SIZE 30000  // Zwiększenie rozmiaru bufora do 30000 bajtów
#define ATTEMPTS 100       // Liczba powtórzeń

char buf[BUFFER_SIZE];    // Bufor na dane

int main(int argc, char **argv)
{
    printf("Klient running...\n");
    struct sockaddr_in endpoint;
    int sdsocket, addrlen, i, received;
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

    // Utworzenie gniazda TCP
    sdsocket = socket(AF_INET, SOCK_STREAM, 0);
    if (sdsocket < 0) {
        perror("socket() nie powiodl sie");
        return 1;
    }

    // Nawiązanie połączenia z serwerem
    if (connect(sdsocket, (struct sockaddr*) &endpoint, addrlen) < 0) {
        perror("connect() nie powiodl sie");
        return 1;
    }


    // Pętla wysyłająca i odbierająca dane
    for (i = 0; i < ATTEMPTS; i++) {
        send(sdsocket, buf, BUFFER_SIZE, 0);
        received = 0;
        while (received < BUFFER_SIZE) {
            int len = recv(sdsocket, buf + received, BUFFER_SIZE - received, 0);
            if (len <= 0) {
                perror("recv() nie powiodl sie");
                break;
            }
            received += len;
        }
    }
     // Zamknięcie gniazda
    close(sdsocket);

    // Pomiar czasu - koniec
    gettimeofday(&time_e, NULL);

    // Obliczenie i wyświetlenie średniego czasu
    printf("czas: %.6f s\n",
        (((double) (time_e.tv_sec - time_b.tv_sec) * 1000000) +
        ((double) (time_e.tv_usec - time_b.tv_usec)))
        / (1000000.0 * ATTEMPTS));

   
    return 0;
}
