#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <sys/time.h>

#define BUFFSZ 1024
#define UDP_PORT 5005
#define TIME_LIMIT 60 /* microseconds! */

/* would be simpler in asm */
union c2li {
  char b[8];
  long int li;
};

int main(int argc, char* argv[]){
  struct sockaddr_in si_srv, si_cli;
  int sock, i = sizeof(struct sockaddr_in), recvd, j;
  long int key[2], conf_msg;
  char buf[BUFFSZ];
  union c2li d;
  struct timeval t;
  /* need blacklisted ips and seeion ips */
  /* srandom(unsigned int seed); */

  key[0] = random();
  key[1] = random();

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
      gettimeofday(&t, NULL);
      conf_msg = si_cli.sin_addr.s_addr ^ (t.tv_usec + key[0]);
      conf_msg ^= key[1];
      
      sendto(sock, &conf_msg, sizeof(conf_msg), 0, (struct sockaddr *) &si_cli, 
	     sizeof(struct sockaddr_in));
    }
    else if( recvd == sizeof(conf_msg) ){
      /* received confirmation from a client */
      
      for(j = 0; j < sizeof(d); j++)
	d.b[j] = buf[j];
      conf_msg = d.li ^ key[1];
      conf_msg ^= si_cli.sin_addr.s_addr;
      gettimeofday(&t, NULL);
      if( (t.tv_usec -  (conf_msg - key[0]) ) < TIME_LIMIT ){
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
