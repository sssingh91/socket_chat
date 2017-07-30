/****************** CLIENT CODE TCP CONNECTION	 ****************/
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <stdbool.h>
#define SERVER_TCP_PORT 3002
#define BUFLEN 10000
int main(int argc, char **argv){
	int sd, port;
	int n, bytes_to_read;
	struct hostent *hp;
	struct sockaddr_in server;
	char *host, *bp, rbuf[BUFLEN], sbuf[BUFLEN];
	bool ifexit = false;

	switch(argc){
	case 2:
		host=argv[1];			
		port=SERVER_TCP_PORT;	     // assigning port number 3002	
		break;
	case 3:
		host=argv[1];			
		port=atoi(argv[1]);
		break;
	default:
		printf("Usage: %s [port]\n", argv[0]);
		exit(1);
	}
// a) int family: Address family as AF_INET for Internet connection using TCP/IP  b) int type: Type as SOCK_STREAM for connection oriented service  c) int protocol: '0' indicates default protocol which is TCP for AF_INET and SOCK_STREAM

	if((sd = socket(AF_INET, SOCK_STREAM, 0))== -1){   // sd = socket descriptor returned by socket call
		printf("cant create socket\n");
		exit(1);
	}

// Bind an address to socket

	server.sin_family= AF_INET;  // family identifier : AF_INET
	server.sin_port = htons(port);  // convert unsigned_short form host to network byte order(big endian)
	printf("server port : %d port in network byte order: %d \n", port, server.sin_port);  
	if((hp = gethostbyname(host))==NULL){
		printf("cant get server address\n");
		exit(1);
	}
	bcopy(hp->h_addr, (char*) &server.sin_addr, hp->h_length);
	if(connect(sd, (struct sockaddr *) &server, sizeof(server))==-1){
		printf("cant connect\n");
		exit(1);
	}
	printf("connected: server address: %s \n", hp->h_name);
	printf("Enter '*' to switch \n");


	while(ifexit == false){                     // connected until '#' entered by client or server 
		do{
			
			bp=sbuf;		    // send buffer
			printf("client:\n");
			fgets(sbuf, sizeof(sbuf), stdin);
			write(sd, sbuf, BUFLEN);	// writing/ sending to client
			if(*sbuf == '*'){		// '*' for switching between server and client
				*sbuf='*';
				*rbuf='*';
				
			}
			if(*sbuf == '#'){		// '#' for closing the connection
				ifexit = true;
				printf("closed by you \n");
				goto cls;
			}
		
		}while(*sbuf != 42);
		
		do{	
			bp=rbuf;			// receive buffer				
			bytes_to_read=BUFLEN;
			while ((n= read(sd, bp, bytes_to_read)) > 0){      // read from server in 'receive buffer'
				bp+=n;
				bytes_to_read-= n;
			}
			printf("server:\n");
			printf("%s\n", rbuf);
			if(*rbuf == '*'){
				*rbuf='*';
				*sbuf='*';
				
			}
			if(*rbuf == '#'){
				ifexit = true;
				printf("closed by server\n");
				goto cls;
			}
		}while(*rbuf != 42);
 
cls:		if(ifexit == true){
			close(sd);					// closing socket
		}
	}
	close(sd);
	return(0);
	
}

