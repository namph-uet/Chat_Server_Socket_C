#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <iostream>

using namespace std;

void * doRecieving(void * sockID){

	int clientSocket = *((int *) sockID);

	while(1){

		char data[1024];
		int read_len = read(clientSocket,data,1024);
		data[read_len] = '\0';
		printf("%s\n",data);

	}

}

int main(){

	int clientSocket = socket(PF_INET, SOCK_STREAM, 0);

	struct sockaddr_in serverAddr;

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(8080);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(connect(clientSocket, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) == -1) return 0;

	printf("Connection established ............\n");

	pthread_t thread;
	pthread_create(&thread, NULL, doRecieving, (void *) &clientSocket );

	while(1){

		char input[1024];
		cin >> input;

		if(strcmp(input,"LIST") == 0){
			write(clientSocket,input,1024);

		}
		if(strcmp(input,"SEND") == 0){

			write(clientSocket,input,1024);
			
			scanf("%s",input);
			write(clientSocket,input,1024);
			
			scanf("%[^\n]s",input);
			write(clientSocket,input,1024);

		}
        else {
            write(clientSocket,input,1024);
        }

	}


}