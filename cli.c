#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/ip.h>

#define BUFFSZ 1024
#define UDP_PORT 5005

void main(void){
  
  struct sockaddr_in si;
  int sock, i, recvd;
  long int conf_msg;
  char buf[BUFFSZ];
  
  if( (sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1 )
    perror("CLI:\tFailed to open socket"), exit(0);

  memset( (char *) &si, 0, sizeof(struct sockaddr_in) );
  buf[0] = buf[1] = 0;
  si.sin_family = AF_INET;
  si.sin_port = htons(UDP_PORT);
  if( inet_aton("127.0.0.1", &(si.sin_addr) ) == 0)
    printf("CLI:\tinet_aton failed\n"), exit(0);
  
  if( (sendto(sock, buf, 2, 0, (struct sockaddr *) &si, 
	      sizeof(struct sockaddr_in))) == -1)
    perror("CLI:\tsendto Failed"), exit(0);
  
  i = sizeof(struct sockaddr_in);
  if( (recvd = 
       recvfrom(sock, &conf_msg, sizeof(conf_msg), 0, 
		(struct sockaddr *) &si, &i)) == -1)
    printf("SRV:\t recvfrom failed\n"), exit(0);

  /* echo the packet */
  sendto(sock, &conf_msg, sizeof(conf_msg), 0, 
	 (struct sockaddr *) &si, sizeof(struct sockaddr_in));

  recvd = recvfrom(sock, &conf_msg, sizeof(conf_msg), 0, 
		   (struct sockaddr *) &si, &i);
  if( !conf_msg ){
    /* session started */
    printf("CLI:\tsession started\n");

  }

  close(sock);
  return;
}
