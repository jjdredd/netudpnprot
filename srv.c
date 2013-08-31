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
#define TIME_LIMIT 1 /* seconds */

union c2i {
  char b[4];
  long int i;
};

int main(int argc, char* argv[]){
  struct sockaddr_in si_srv, si_cli;
  int sock, i = sizeof(struct sockaddr_in), recvd, j, key, conf_msg;
  char buf[BUFFSZ];
  union c2i d;
  /* need blacklisted ips and seeion ips */
  /* srandom(unsigned int seed); */

  key = random();
  
  if( (sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1 )
    perror("SRV\terror openong a socket"), exit(0);

  memset((char *) &si_srv, 0, sizeof(struct sockaddr_in));
  si_srv.sin_family = AF_INET;
  si_srv.sin_port = htons(UDP_PORT);
  si_srv.sin_addr.s_addr = htonl(INADDR_ANY);
  if( bind(sock, (struct sockaddr *) &si_srv, i) == -1 )
    perror("SRV:\tbind failed"), exit(0);

  while(1){
    i = sizeof(struct sockaddr_in);
    if( (recvd = 
	 recvfrom(sock, buf, BUFFSZ, 0, (struct sockaddr *) &si_cli, &i)) 
	== -1)
      printf("SRV:\t recvfrom failed\n"), exit(0);

    /* if this client ip is not blacklisted */

    /*    if this client is not in the session */
    
    buf[recvd] = 0;
 
    /* printf("Received packet from %s:%d\nData: %s\n\n", 
              inet_ntoa(si_cli.sin_addr), ntohs(si_cli.sin_port), buf);
       printf("%i %i %i\n", buf[0], buf[1], recvd); */

    if( !(buf[0] && buf[1]) && (recvd == 2) ){
      /* this client connecting the first time
	 send the confirmation request */
      conf_msg = si_cli.sin_addr.s_addr ^ time(NULL);
      /* use gettimeofday() for usec instead? */
      conf_msg ^= key;
      
      sendto(sock, &conf_msg, sizeof(conf_msg), 0, (struct sockaddr *) &si_cli, 
	     sizeof(struct sockaddr_in));
    }
    else if( recvd == sizeof(conf_msg) ){
      /* received confirmation from a client */
      
      for(j = 0; j < sizeof(d); j++)
	d.b[j] = buf[j];
      conf_msg = d.i ^ key;
      conf_msg ^= si_cli.sin_addr.s_addr;

      if( (time(NULL) -  conf_msg) < TIME_LIMIT ){
	/* add the client ip to the session list */
	printf("SRV:\t%s accepted to session list\n",
	       inet_ntoa(si_cli.sin_addr));
	/* send confirmation to the client */
	conf_msg = 0;
	sendto(sock, &conf_msg, sizeof(conf_msg), 0, 
	       (struct sockaddr *) &si_cli, sizeof(struct sockaddr_in));
      }
      /* add to ignore list as a doser
	 else {
	 
	 }*/
    }
  }

  close(sock);
  return;
}