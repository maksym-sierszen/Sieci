#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <unistd.h>

// char targetIP[]="150.254.77.129";
char targetIP[]="127.0.0.1"; // Adres IP docelowy (tutaj ustawiony na localhost dla testów)
#define MAX_SIZE 11 // Maksymalny rozmiar bufora

int main(int argc, char const *argv[])
{
    struct sockaddr_in target_addr; // Struktura adresowa dla adresu docelowego
    struct sockaddr_in source_addr; // Struktura adresowa dla adresu źródłowego
    struct sockaddr_in endpoint_addr; // Struktura adresowa dla odbioru potwierdzenia
    unsigned int target_port; // Port docelowy
    unsigned int source_port; // Port źródłowy
    int socket_fd, result; // Deskryptory gniazd
    unsigned int index = 485950; // Numer indeksu
    unsigned char message[MAX_SIZE]; // Bufor na wiadomość
    char response[512]; // Bufor na odpowiedź
    

    // Pobieranie portów od użytkownika
    printf("TARGET PORT: ");
    scanf("%u", &target_port);
    printf("PORT LISTENING: ");
    scanf("%u", &source_port);

    
    sprintf((char *)message, "%d\n", index); // Formatowanie numeru indeksu do postaci ASCII i dodanie znaku nowej linii
    unsigned int index_htonl = htonl(index);   // Konwersja numeru indeksu do formatu sieciowego (big endian) przy uzyciu htonl
    memcpy(message + 7, &index_htonl, sizeof(index_htonl));  // Bezpośrednie kopiowanie przekonwertowanej liczby do bufora wynikowego

    // ------- WYRZUCENIE DANYCH NA EKRAN --------------------------------
    printf("INDEX: %s\nSIZE: %ld\n", message, sizeof(message));

    for (int i = 0; i<sizeof(message); i++) {
        printf("%02X ", message[i]);
    };
    // ------- WYRZUCENIE DANYCH NA EKRAN --------------------------------

   
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);  // Tworzenie gniazda UDP
    // Ustawianie adresu docelowego
    target_addr.sin_family = AF_INET;
    target_addr.sin_port = htons(target_port);
    target_addr.sin_addr.s_addr = inet_addr(targetIP);

    // Wysyłanie pakietu UDP
    result = sendto(socket_fd,
               message,
               11,
               0,
               (struct sockaddr*) &target_addr,
               sizeof(target_addr));
    if (result != 11) printf("!sendto() FAILED\n");
    else printf("MESSAGE SENT\n");
    close(socket_fd); // Zamknięcie gniazda po wysłaniu

    
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0); // Tworzenie nowego gniazda do odbioru potwierdzenia
    // Ustawianie adresu źródłowego
    source_addr.sin_family = AF_INET;
    source_addr.sin_port = htons(source_port);
    source_addr.sin_addr.s_addr = INADDR_ANY;

    // Przypisanie gniazda do adresu lokalnego
    if (bind(socket_fd, (struct sockaddr*) &source_addr, sizeof(struct sockaddr_in)) < 0) {
        printf("!bind() FAILED\n");
        return 1;
    }

    // Odbiór potwierdzenia
    socklen_t addrlen = sizeof(struct sockaddr_in);
    recvfrom(socket_fd, 
        response, 
        sizeof(response), 
        0, 
        (struct sockaddr*) &endpoint_addr, 
        &addrlen);
    printf("POTWIERDZENIE: %s\nOd: %s\n", response, inet_ntoa(endpoint_addr.sin_addr));
    close(socket_fd); // Zamknięcie gniazda po odbiorze

    return 0;
}
