#include <iostream>
#include "sender.h"

using namespace std;

Sender::Sender(int size, char *msg){
	message = msg;
	length = size;
	lastSent = 0;
	lastAck = 0;
}

void Sender::setMessage(int size, char *msg){
	message = msg;
	length = size;
}

int Sender::slidingWindow(){
	return 0;
}

int Sender::initialMessage(){
	return 0;
}

void Sender::sendMessage(char* buffer, char* sender_ip, char* p){

	struct addrinfo hints, *server_info;

 	memset(&hints, 0, sizeof hints);
 	hints.ai_family = AF_INET; //IPv4
 	hints.ai_socktype = SOCK_DGRAM; // UDP socket

 	int status = getaddrinfo(sender_ip, p, &hints, &server_info);
 	if (status != 0)
 	{
 		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
 		exit(1);
 	}

 	int newSock;	//newsocket to send info to
 	struct addrinfo *ptr = server_info; //ptr to current struct addrinfo
 	while (ptr != NULL){				//attempt to build a socket for the senders ip and port
 		newSock = socket(ptr->ai_family, ptr->ai_socktype, 0);
 		if (newSock != -1){
 			break;
 		}
 		ptr = ptr->ai_next;
 	}
 	if (ptr == NULL){
 		fprintf(stderr, "client: failed to create socket\n");
 		exit(2);
 	}

 	int numbytes = sendto(newSock, buffer, MAXBUFLEN, 0, ptr->ai_addr, ptr->ai_addrlen);		//send out last message
 	if ((numbytes) == -1)
 	{
 		perror("client: sendto");
 		exit(1);
 	}
}

int main (void){

	return 0;
}
