#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mongo.h>

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

   mongo conn[1];

   int status = mongo_connect( conn, "192.168.10.18", 27017 );

 if( status != MONGO_OK ) {
      switch ( conn->err ) {
        case MONGO_CONN_SUCCESS:    printf( "connection succeeded\n" ); break;
        case MONGO_CONN_NO_SOCKET:  printf( "no socket\n" ); return 1;
        case MONGO_CONN_FAIL:       printf( "connection failed\n" ); return 1;
        case MONGO_CONN_NOT_MASTER: printf( "not master\n" ); return 1;
      }
  }


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
	y=0;
	char* temp = mesg;
	for (x=0;x<14;x++){
		if (*temp){
			if ((ptr=strstr(temp,"    "))){
				*ptr=0;
				chunks[x]=temp;
				chunks[x+1]=ptr+4;
				temp=ptr+4;
			}
			else{
				temp=mesg+n;
			}
		}
	}
	if (chunks[0] == '\1'){
		bson b[1];
		bson_init(b);
		bson_append_new_oid(b,"_id");
		bson_append_string(b,"server",chunks[1]);
		bson_append_int(b,"tstamp",strtol(chunks[2],NULL,10));
		bson_append_int(b,"code",atoi(chunks[3]));
		bson_append_string(b,"catcher",chunks[4]);
		bson_append_string(b,"sapi",chunks[5]);
		bson_append_string(b,"filename",chunks[6]);
		bson_append_int(b,"lineno",strtol(chunks[7],NULL,10));
		bson_append_string(b,"classname",chunks[8]);
		bson_append_string(b,"function",chunks[9]);
		bson_append_string(b,"message",chunks[10]);
		bson_append_string(b,"request_id",chunks[11]);
		bson_append_string(b,"hostname",chunks[12]);
		bson_append_string(b,"uri",chunks[13]);
		bson_finish(b);

		mongo_insert(conn,"php_errors.log",b,NULL);

		bson_destroy(b);
	}
	
   }

    mongo_destroy( conn );
}
