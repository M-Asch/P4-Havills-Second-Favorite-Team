#include <iostream>
#include "sender.h"

using namespace std;

Sender::Sender(int size, char *msg){
	bool buff[size];
	message = msg;
	buffer = buff;
	length = size;
	lastSent = 0;
	lastAck = 0;
}

struct sockaddr_in Sender::makeSocket(void){
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVERPORT);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	memset(server_addr.sin_zero, 0, sizeof server_addr.sin_zero);

	return server_addr;
}

void Sender::setMessage(int size, char *msg){
	message = msg;
	length = size;
}

void Sender::updateSent(int newlySent){
	lastSent = newlySent;
}

void Sender::updateAck(int newAck){
	lastAck = newAck;
}

int Sender::getSent(){
	return lastSent;
}

int Sender::getAck(){
	return lastAck;
}

char* Sender::getMessage(){
	return message;
}

int Sender::slidingWindow(char* hostname){

	int LISTENING = 1;

	//=============================
	//Find and Bind to reciever port
	//=============================
	struct addrinfo hints, *server_info;

 	memset(&hints, 0, sizeof hints);
 	hints.ai_family = AF_INET; //IPv4
 	hints.ai_socktype = SOCK_DGRAM; // UDP socket

 	int status = getaddrinfo(hostname, SERVERPORTS, &hints, &server_info);
 	if (status != 0)
 	{
 		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
 		exit(1);
 	}

 	int sockfd;
 	struct addrinfo *ptr = server_info; //ptr to current struct addrinfo
 	while (ptr != NULL)
 	{
 		sockfd = socket(ptr->ai_family, ptr->ai_socktype, 0);
 		if (sockfd != -1)
 		{
 			break;
 		}
 		ptr = ptr->ai_next;
 	}

 	if (ptr == NULL)
 	{
 		fprintf(stderr, "client: failed to create socket\n");
 		exit(2);
 	}
 	
 	
 	//=============================
 	//Bind sender port
 	//=============================
 	struct sockaddr_in my_addr;
  	my_addr.sin_family = AF_INET;
  	my_addr.sin_port = htons(9091);
  	my_addr.sin_addr.s_addr = INADDR_ANY;
  	memset(my_addr.sin_zero, 0, sizeof my_addr.sin_zero);

	//Binds socket for client
 	if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof my_addr) == -1){
		close(sockfd);
		perror("client: bind failed");
		exit(1);
	}
	
	//send initial ack message to reciever
	char initM[8];
	memset(initM, 0, 8);
	initialMessage(initM);
	
	int numbytes = sendto(sockfd, initM, sizeof(initM), 0, ptr->ai_addr, ptr->ai_addrlen);
 	if ((numbytes) == -1)
 	{
 		perror("client: sendto");
 		exit(1);
 	}
 	
 	//
 	struct pollfd pfds[2];

	while (LISTENING)
	{
		char buffer[8]; //buffer to hold recieved message
		memset(buffer, 0, 1034);
 		struct sockaddr_storage sender_addr;      // sender's address (may be IPv6)
		socklen_t addr1_len = sizeof sender_addr;  // length of this address

		pfds[0].fd = sockfd;
		pfds[0].events = POLLIN;

		pfds[1].fd = 0; //cin
		pfds[1].events = POLLIN;
		
		int num_events = poll(pfds, 2, -1);
		
		if (num_events != 0) {
			//printf("Type message: "); //Prompts client to enter message to send to chat partner
			int pollin_happened1 = pfds[0].revents & POLLIN;
			
			if (pollin_happened1){
				//Stage 3: check ack
				char recieve[8];
				memset(recieve, 0 , 8);
        		int numbytes = recvfrom(sockfd, recieve, 8, 0, (struct sockaddr *)&sender_addr, &addr1_len);
		    	if (numbytes == -1)
		    	{
			     		perror("recvfrom");
			     		exit(1);
		   		}

				recieve[numbytes] = '\0';
				printf("reciever: \"%s\"\n", recieve);
			}
		}
	}

	return 0;
}

char* Sender::initialMessage(char* initial){
	string init = "00000100";
	for (int i = 0; i< 8;i++){
		initial[i] = init[i];
	}
	return initial;
}

int Sender::sendMessage(char* buffer, char* sender_ip, char* p, int send){
	return 0;
}

int main (void){

	int size = 20;
	char m[20] = "red7.cs.denison.edu";
	Sender msg(size, m);
	msg.updateSent(52);
	
	msg.slidingWindow(m);
	return 0;
}
