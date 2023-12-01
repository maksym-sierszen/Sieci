#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

struct sockaddr_in endpoint;
FILE *flog;
char myhostname[1024];

int main(int argc, char **argv) {
 long lnIn1, lnIn2, lhIn1, lhIn2, lhOut, lnOut;
 int sdServerSocket, sdConnection, retval;
 socklen_t sin_size;
 struct sockaddr_in incoming;
 struct hostent *heLocalHost;
 char sign;

 sin_size = sizeof(struct sockaddr_in);

 sdServerSocket = socket(PF_INET, SOCK_STREAM, 0);
 gethostname(myhostname, 1023);
 heLocalHost = gethostbyname(myhostname);

 endpoint.sin_family = AF_INET;
 endpoint.sin_port = htons(4444);
 endpoint.sin_addr = *(struct in_addr*) 
 heLocalHost->h_addr;
 memset(&(endpoint.sin_zero),0,8);

 printf("slucham na %s:%d\n",
 inet_ntoa(endpoint.sin_addr),
 ntohs(endpoint.sin_port));

 retval = bind(sdServerSocket,
 (struct sockaddr*) &endpoint,
 sizeof(struct sockaddr));

 if (retval < 0) {
 printf("bind nie powiodl sie\n");
 return 1;
 }

 listen(sdServerSocket, 10);

 sin_size = sizeof(struct sockaddr_in);
 while ((sdConnection =

 accept(sdServerSocket,
 (struct sockaddr*) &incoming,
 &sin_size))

 > 0) {
 printf("Polaczenie z %s:%d\n",
 inet_ntoa(incoming.sin_addr),
 ntohs(incoming.sin_port));

 if (recv(sdConnection, &lnIn1, sizeof(long),0) 
 != sizeof(long)) {
 printf("pierwszy recv nie powiodl sie\n");
 close(sdConnection);
 continue;
 }
 lhIn1 = ntohl(lnIn1);

 lhOut = random();
 lnOut = htonl(lhOut);

 if (send(sdConnection, &lnOut, sizeof(long), 0) 
 != sizeof(long)) {
 printf("send nie powiodl sie\n");
 close(sdConnection);
 continue;
 }

 if (recv(sdConnection, &lnIn2, sizeof(long), 0) 
 != sizeof(long)) {
 printf("drugi recv nie powiodl sie\n");
 close(sdConnection);
 continue;
 }

 lhIn2 = ntohl(lnIn2);

 flog = fopen("zad.txt","a");
 if (lhIn2 == lhOut + 1) sign = '+'; 
 else sign = '-';
 fprintf(flog,"%c %ld from %s:%d : %ld, %ld\n",
 sign,
 lhIn1,
 inet_ntoa(incoming.sin_addr),
 ntohs(incoming.sin_port),
 lhOut,
 lhIn2);

 close(sdConnection);
 fflush(flog);
 fclose(flog);
 }

 printf("Blad sieci\n");
 fclose(flog);
 return 0;
}