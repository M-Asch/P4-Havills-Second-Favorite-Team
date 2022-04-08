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

int Sender::slidingWindow(){
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


	if (send == 0)
 		return(newSock);	
	
 	int numbytes = sendto(newSock, buffer, MAXBUFLEN, 0, ptr->ai_addr, ptr->ai_addrlen);		//send out last message
 	if ((numbytes) == -1)
 	{
 		perror("client: sendto");
 		exit(1);
 	}
 return 0;
}

int main (void){

	int size = 10;
	char m[10] = "abcdehikd";
	Sender msg(size, m);
	msg.updateSent(52);
	
	char initialM[8];
	msg.initialMessage(initialM);
	return 0;
}
