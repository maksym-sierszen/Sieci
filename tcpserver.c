#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUFFER_SIZE 30000  // Zwiększenie rozmiaru bufora do 30000 bajtów
#define ATTEMPTS 100       // Liczba powtórzeń

char buf[BUFFER_SIZE];    // Bufor na dane

int main(int argc, char **argv)
{
    printf("Server running...\n");
    struct sockaddr_in myaddr, endpoint;
    int sdsocket, sdconnection, addrlen, received, i;

    if (argc < 2) {
        printf("podaj numer portu jako parametr\n");
        return 1;
    }

    // Utworzenie gniazda TCP
    sdsocket = socket(AF_INET, SOCK_STREAM, 0);

    addrlen = sizeof(struct sockaddr_in);

    // Konfiguracja adresu serwera
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(atoi(argv[1]));
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Przypisanie adresu do gniazda
    if (bind(sdsocket, (struct sockaddr*) &myaddr, addrlen) < 0) {
        printf("bind() nie powiodl sie\n");
        return 1;
    }

    // Ustawienie gniazda w tryb nasłuchiwania
    if (listen(sdsocket, 10) < 0) {
        printf("listen() nie powiodl sie\n");
        return 1;
    }

    // Akceptacja połączenia od klienta
    sdconnection = accept(sdsocket, (struct sockaddr*) &endpoint, &addrlen);
    if (sdconnection < 0) {
        perror("accept() nie powiodl sie");
        return 1;
    }

    // Pętla odbierająca dane od klienta
    for (i = 0; i < ATTEMPTS; i++) {
        received = 0;
        while (received < BUFFER_SIZE) {
            int len = recv(sdconnection, buf + received, BUFFER_SIZE - received, 0);
            if (len <= 0) {
                perror("recv() nie powiodl sie");
                break;
            }
            received += len;
        }

        // Wysyłanie danych z powrotem do klienta
        send(sdconnection, buf, received, 0);
    }

    // Zamknięcie połączenia
    close(sdconnection);
    close(sdsocket);
    return 0;
}
