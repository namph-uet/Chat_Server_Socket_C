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
#include <string>
#include <bits/stdc++.h> 
#include <iostream>

int clientCount = 0;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

struct client{
	int index;
	int sockID;
	struct sockaddr_in clientAddr;
	int len;
    char userID[50];

};

struct client Client[1024];
pthread_t thread[1024];

void removeClient(int index) {
	for(int i = index; i <= clientCount; i++) {
		Client[i] = Client[i+1];
	}
	clientCount--;
}

void * doNetworking(void * ClientDetail){

	struct client* clientDetail = (struct client*) ClientDetail;
	int index = clientDetail -> index;
	int clientSocket = clientDetail -> sockID;
    bool typing_user_id = true;
	bool typing_mess = false;
    const std::string LOGGED_IN = "LOGGED IN";
	const std::string HAVE_NOT_USER = "HAVE NOT USER";
	const std::string SENT = "SENT YOUR MESSGAGE";
	
	while(1){

		char data[1024];
		bool have_user = false;
		int read_len = read(clientSocket,data,1024);
		data[read_len] = '\0';

        std::cout << "Received a message from the client " << clientDetail->userID
          <<": " << data << std::endl;

		char output[1024];

        if(typing_user_id) {
            strncpy(clientDetail->userID, data, read_len);
            write(clientSocket, LOGGED_IN.c_str(), LOGGED_IN.length());
            std:: cout << "Client " << clientDetail->userID << " connected" << std::endl;
            typing_user_id = false;
        }

		std::cout << clientCount << std::endl;

		if(strcmp(data,"LIST") == 0){
			for(int i = 0 ; i < clientCount; i ++){
				std::cout << "i: " << i << std::endl;
                write(clientSocket, Client[i].userID, 50);
			}
		}
		if(strcmp(data,"SEND") == 0){

			char to_user[50]; 
			bzero(data, 1024);
			read_len = read(clientSocket,to_user,50);
			to_user[read_len] = '\0';
			typing_mess = true;
			std::cout << "to user: " << to_user << std::endl;
		}
		else if(typing_mess) {
			std::cout << "message: " << data << std::endl;

			for(int i = 0; i < clientCount; i++) {
				if(strcmp(Client[i].userID, data) == 0) {
					write(Client[i].sockID,data,1024);
					write(clientSocket,SENT.c_str(),SENT.length());
					have_user = true;
					break;
				}
			}
			
			if(!have_user) write(clientSocket,HAVE_NOT_USER.c_str(),HAVE_NOT_USER.length());
			typing_mess = false;
		}

		if(read_len == 0) {
		 /* close socket and clean up */
		std::cout << "client " << clientDetail->userID << " close the connection\n";
			removeClient(index);
			close(clientSocket);
			pthread_exit(0);
		}
	}

    /* close socket and clean up */
    std::cout << "client " << clientDetail->userID << " close the connection\n";
	removeClient(index);
	close(clientSocket);
    pthread_exit(0);
	return NULL;

}

int main(){

	int serverSocket = socket(PF_INET, SOCK_STREAM, 0);

	struct sockaddr_in serverAddr;

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(8080);
	serverAddr.sin_addr.s_addr = htons(INADDR_ANY);


	if(bind(serverSocket,(struct sockaddr *) &serverAddr , sizeof(serverAddr)) == -1) return 0;

	if(listen(serverSocket,1024) == -1) return 0;

	printf("Server started listenting on port 8080 ...........\n");

	while(1){

		Client[clientCount].sockID = accept(serverSocket, (struct sockaddr*) &Client[clientCount].clientAddr, (socklen_t *) &Client[clientCount].len);
		Client[clientCount].index = clientCount;
		
		pthread_create(&thread[clientCount], NULL, doNetworking, (void *) &Client[clientCount]);
		clientCount ++;
		std::cout <<"count: " << clientCount << std::endl;
	}

	for(int i = 0 ; i < clientCount ; i ++)
		pthread_join(thread[i],NULL);

}