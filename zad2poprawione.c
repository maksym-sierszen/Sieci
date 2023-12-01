#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_SIZE 11 // Maksymalny rozmiar bufora
#define SERVER_IP "150.254.77.129" // Adres IP serwera

int main() {
    int socket_fd, result;
    struct sockaddr_in server_addr;
    unsigned int target_port;
    unsigned int index = 485950; 
    unsigned char message[MAX_SIZE];
    char response[512]; 

    // Pobieranie numeru portu
    printf("TARGET PORT: ");
    scanf("%u", &target_port);

    // Przygotowanie wiadomości
    sprintf((char *)message, "%d\n", index); // Formatowanie numeru indeksu do postaci ASCII
    unsigned int index_htonl = htonl(index); // Konwersja numeru indeksu do formatu sieciowego
    memcpy(message + 7, &index_htonl, sizeof(index_htonl)); // Kopiowanie przekonwertowanej liczby do bufora

    // Tworzenie gniazda UDP
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        perror("Error creating socket");
        return 1;
    }

    // Ustawianie adresu serwera
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(target_port);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Wysyłanie pakietu UDP
    result = sendto(socket_fd, 
                message, 
                MAX_SIZE, 
                0, 
                (struct sockaddr*)&server_addr, 
                sizeof(server_addr));
    if (result != MAX_SIZE) {
        perror("!sendto() FAILED");
    } else {
        printf("MESSAGE SENT\n");
    }

    // Odbiór potwierdzenia
    socklen_t addrlen = sizeof(struct sockaddr_in);
    recvfrom(socket_fd, 
        response, 
        sizeof(response), 
        0, 
        (struct sockaddr*)&server_addr, 
        &addrlen);
    printf("CONFIRMATION: %s\nFROM: %s\n", response, inet_ntoa(server_addr.sin_addr));

    close(socket_fd); // Zamknięcie gniazda

    return 0;
}
