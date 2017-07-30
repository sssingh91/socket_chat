/****************** SERVER CODE TCP CONNECTION (MULTI CLIENT)	 ****************/
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include<pthread.h> 
#define SERVER_TCP_PORT 3002 
#define BUFLEN 10000


int main(int argc, char **argv){
	int sd, new_sd, client_len, port, *nw;
	void *multi_client(void *nw);
	
	struct sockaddr_in server, client;
	switch(argc){
	case 1:
		port=SERVER_TCP_PORT;        // assigning port number 3002	
		break;
	case 2:
		port=atoi(argv[1]);
		break;
	default:
		printf("Usage: %s [port]\n", argv[0]);
		exit(1);
	}

// a) int family: Address family as AF_INET for Internet connection using TCP/IP  b) int type: Type as SOCK_STREAM for connection oriented service  c) int protocol: '0' indicates default protocol which is TCP for AF_INET and SOCK_STREAM

	if((sd = socket(AF_INET, SOCK_STREAM, 0))== -1){  // sd = socket descriptor returned by socket call
		printf("cant create socket\n");
		exit(1);
	}
// Bind an address to socket

	server.sin_family= AF_INET;    // family identifier : AF_INET
	server.sin_port = htons(port);  // convert unsigned_short form host to network byte order(big endian)
	server.sin_addr.s_addr = htonl(INADDR_ANY); // assigning multiple IP address
	printf("port %d\n", server.sin_port);
	if(bind(sd, (struct sockaddr *) &server, sizeof(server)) == -1){  // "&server" is pointer to address structuture which contains local IP address and port number 
               	printf("cant bind name to socket\n");
		exit(1);
	}
	if(listen(sd, 15)== 0){        //  queqe length = 5
		printf("Listening\n");
	}
	else
		printf("Error\n");
	
	printf("Enter '*' to switch:\n ");
	client_len = sizeof(client);

// client connected to various server ports using accept

	while( (new_sd = accept(sd, (struct sockaddr *)&client, &client_len)) ){ 
		printf("new client connected to server port: %d\n new client connected to server IP address : %d \n", client.sin_port, client.sin_addr.s_addr);
		printf("new client sd: %d \n \n", new_sd);
         	
//***********************multi client using pthread by creating multiple threads of process***********************************************//
	        pthread_t thread;  
	        nw = malloc(1);
	        *nw = new_sd;
	         
	        if(pthread_create( &thread , NULL ,  multi_client , (void*) nw) < 0)  // calling process multi_client
	        {
	            perror("could not create thread");
	            return 1;
	        }
         	

		
	}
clt:	close(sd);		// closing server socket
	return(0);

}


void *multi_client(void *nw) {
	int n, bytes_to_read;
	int new_sd= *(int*)nw;
	char *bp, rbuf[BUFLEN], sbuf[BUFLEN];
	bool ifexit = false;
	do{
		do{
			
			bp=rbuf;                     			 // receive buffer
			bytes_to_read = BUFLEN;
			while ((n= read(new_sd, bp, bytes_to_read)) > 0){    // read from client in 'receive buffer'
				bp+=n;
				bytes_to_read-= n;
			}
			printf("client %d: \n %s\n", new_sd, rbuf);

			if(*rbuf == '*'){                                 // '*' for switching between server and client
				*rbuf='*';
				*sbuf='*';
					
			}

			if(*rbuf == '#'){				  // '#' for closing the connection
				ifexit = true;
				printf("closed by client: %d\n", new_sd);
				goto cls;				
			}			
		}while(*rbuf != 42);
			

		do{
			bp=sbuf;                                   	   // sending buffer
			printf("server: \n");			
			fgets(sbuf, sizeof(sbuf), stdin);
			write(new_sd, sbuf, BUFLEN);                       // writing/ sending to client
			if(*sbuf == '*'){
				*sbuf='*';
				*rbuf='*';
					
			}
			
		
			if(*sbuf == '#'){
				ifexit = true;
				printf("closed by server\n");
				goto cls;
			}
		}while(*sbuf != 42);
cls:		if(ifexit == true){
			close(new_sd);                                     // closing client connection
		}
	}while(ifexit == false);
	
}	











































