#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char**argv)
{
   int sockfd,n;
   struct sockaddr_in servaddr,cliaddr;
   socklen_t len;
   char mesg[35000];

   sockfd=socket(AF_INET,SOCK_DGRAM,0);

   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
   servaddr.sin_port=htons(11111);
   bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
	char* chunks[14];

   for (;;)
   {
	char x,y;
	char * ptr;
	for (x=0;x<14;x++){
		chunks[x]=NULL;
	}
      len = sizeof(cliaddr);
      n = recvfrom(sockfd,mesg,35000,0,(struct sockaddr *)&cliaddr,&len);
      mesg[n] = 0;
	printf("%s\n",mesg);
	
   }

}
