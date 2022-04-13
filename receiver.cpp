// receiver.cpp

#include <iostream>
#include "receiver.h"

using namespace std;


Receiver::Receiver(){
  lastRecieve = 0;
  lastAcceptable = 0;
  seen = new int[1024];
}

int Receiver::getLastRecieve(){
	return lastRecieve;
}

int Receiver::getLastAcceptable(){
	return lastAcceptable;
}

int* Receiver::getSeen(){
	return seen;
}

void Receiver::setSeen(){
	seen = {};
}

int Receiver::initialReceive(char buffer[], int sockfd, struct addrinfo *ptr){

    unsigned long seq = ((buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3]);
    unsigned short ack = (buffer[4]);
    unsigned short control = (buffer[5]);
    unsigned short length = (buffer[6] << 8 | buffer[7]);


    if(control == 1 && ack == 0 && length == 0){
      sendAck(seq, sockfd, ptr);
    }
    else{
      return -1;
    }

  	return 0;
}


// char* ip, char* port
int Receiver::sendAck(unsigned long seq, int sockfd, struct addrinfo *ptr){
  char buffer[8];
  memset(buffer, 0, 8);

	unsigned char control = 0x00;
  if (seq == 0){
    control = 0x01;
  }
  unsigned char ack = 0x01;
	unsigned short length = 0;

	buffer[0] = (seq >> 24)& 0xff; //(00000000) 00000000 00000000 00000000
	buffer[1] = (seq >> 16)& 0xff; //00000000 (00000000) 00000000 00000000
	buffer[2] = (seq >> 8)& 0xff; //00000000 00000000 (00000000) 00000000
	buffer[3] = seq & 0xff;		//00000000 00000000 00000000 (00000000)

	buffer[4] = ack &0xff; //(00000000) 00000001 this is the ACK
	buffer[5] = control &0xff; //00000000 (00000001) this is the CONTROL

	buffer[6] = (length >> 8) & 0xff;
	buffer[7] = length & 0xff;

  int numbytes = sendto(sockfd, buffer, sizeof(buffer), 0, ptr->ai_addr, ptr->ai_addrlen);
 	if ((numbytes) == -1)
 	{
 		return -1; // shows error occured in the sendto
 	}

  return 0;
}


void Receiver::receiveMessage(int sockfd, struct addrinfo *ptr){
  char ip[16];
	char portC[6];
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
  printf("balls\n");
	//check to see if clients need to be updated, fill in client information
	memcpy(ip, sender_ip_string, sizeof(sender_ip_string));			//save ip
	memcpy(portC, p, sizeof(p));		//and port string

  if(initialReceive(buffer, sockfd, ptr) == -1){
    perror("initialReceive");
    exit(1);
  }

	while (LISTENING == 1){

	}
}


int main(int argc, char **argv){

  int sockfd;
  struct addrinfo *temp;

	struct addrinfo hints, *server_info;   	//Sets up socket for client to send messages

	memset(&hints, 0, sizeof hints);
 	hints.ai_family = AF_INET; //IPv4
 	hints.ai_socktype = SOCK_DGRAM; // UDP socket

	int status = getaddrinfo(argv[1], SERVERPORTS, &hints, &server_info);
 	if (status != 0)
 	{
 		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
 		exit(1);
 	}

	//int sockfd;
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

	temp = ptr;					//taking the addr information, and inserting it into the global variables
  	temp->ai_addr = ptr->ai_addr;
  	temp->ai_addrlen = ptr->ai_addrlen;

	//Sets up socket for client for recieving message
  	struct sockaddr_in my_addr;
  	my_addr.sin_family = AF_INET;
  	my_addr.sin_port = htons(SERVERPORT); //SERVERPORTS
  	my_addr.sin_addr.s_addr = INADDR_ANY;
  	memset(my_addr.sin_zero, 0, sizeof my_addr.sin_zero);


	// ALL THE RECEIVING AND CHECKING IS HANDLED THROUGH THIS FUNCTION
 	Receiver r;
	r.receiveMessage(sockfd, ptr);


  return 0;
}
