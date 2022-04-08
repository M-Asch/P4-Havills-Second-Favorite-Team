// receiver.cpp

#include <iostream>
#include "receiver.h"

using namespace std;


Receiver::Receiver(){
  lastRecieve = 0;
  lastAcceptable = 0;
  seen[] = {};
}

int Receiver::getLastRecieve(){
	return lastRecieve;
}

int Receiver::getLastAcceptable(){
	return lastAcceptable;
}

int Receiver::getSeen(){
	return seen;
}

void Receiver::setSeen(){
	seen[] = {};
}

int Receiver::initialReceive(char buffer[], char ip[], char port[]){
  
	
  
  
  	return 0;
}


// int Receiver::waitForMessage(){
//   return 0;
// }


int Receiver::sendAck(int seg, char ip[], char port[]){
  return 0;
}


void Receiver::receiveMessage(){
  	char ip[16];
	char port[6];
	int LISTENING = 1;
	  
	//Prepare to recieve message
	char buffer[MAXBUFLEN];                   // buffer to hold received message
	struct sockaddr_storage sender_addr;      // sender's address (may be IPv6)
	socklen_t addr1_len = sizeof sender_addr;  // length of this address

	// wait for a message
	int numbytes = recvfrom(sockfd, buffer, MAXBUFLEN - 1 , 0, (struct sockaddr *)&sender_addr, &addr1_len);//receive message information
	if (numbytes == -1)
	{
		perror("recvfrom");
		exit(1);
	}

	//Get info about sender socket
	char sender_ip_string[INET6_ADDRSTRLEN];
	inet_ntop(sender_addr.ss_family, &(((struct sockaddr_in*)&sender_addr)->sin_addr), sender_ip_string, INET6_ADDRSTRLEN);	//recieve info about sender

	struct sockaddr_in *s = (struct sockaddr_in *)&sender_addr;			//sockaddr_in storage for the sender socket
	int port = ntohs(s->sin_port);							//this is used to get the port
	char p[6];			//used to store the port string

	sprintf(p, "%d", port);					//turn port into string

	buffer[numbytes] = '\0'; // add null terminator for printing (finalize message)

	//check to see if clients need to be updated, fill in client information
	memcpy(ip, sender_ip_string);			//save ip
	memcpy(port, p);		//and port string


	if(initialReceive(buffer, ip, port) == -1){
		perror("initialRecieve");
		exit(1);
	}


	while (LISTENING == 1){
		
	}
  

}
