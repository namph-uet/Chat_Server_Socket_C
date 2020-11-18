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
		cout << "Message received: " << data <<endl;

		if(read_len == 0) {
			 /* close socket and clean up */
			cout <<"server close the connection\n";
			close(clientSocket);
			pthread_exit(0);
		}
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
		bool typing_mess = false;
		cin.getline(input,'\n');
		cin.clear();

		if(strcmp(input,"LIST") == 0){
			cout << "Get list user request\n"; 
			write(clientSocket,input,1024);
		}
		else if(strcmp(input,"SEND") == 0){

			write(clientSocket,input,1024);
			cout << "Enter userId to send meassage: ";

			cin.getline(input,'\n');
			write(clientSocket,input,1024);

			cout << "Enter meassage: ";
			char message [1024];

			cin.getline(message,'\n');
			write(clientSocket,message,sizeof(message));
		}
        else {
            write(clientSocket,input,1024);
        }

	}


}