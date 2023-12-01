#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    long index = 485950;
    long received_number, modified_number;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(4444);
    //server_addr.sin_addr.s_addr = inet_addr("150.254.77.129"); // Adres serwera
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); 

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error connecting to server");
        return 1;
    }

    index = htonl(index);
    send(sockfd, &index, sizeof(index), 0);

    recv(sockfd, &received_number, sizeof(received_number), 0);
    received_number = ntohl(received_number);

    modified_number = htonl(received_number + 1);
    send(sockfd, &modified_number, sizeof(modified_number), 0);

    close(sockfd);
    return 0;
}
