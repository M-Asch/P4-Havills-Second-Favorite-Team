#include <iostream>
#include "sender.h"

using namespace std;

Sender::Sender(int size, char *msg){
	message = msg;
	tracking = new bool [size/MAXDATALENGTH + 1];
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
	int sockfd;
	struct addrinfo *temp;

	//=============================
	//Find and Bind to reciever port
	//=============================
	struct addrinfo hints, *server_info;   	//Sets up socket for client to send messages

	memset(&hints, 0, sizeof hints);
 	hints.ai_family = AF_INET; //IPv4
 	hints.ai_socktype = SOCK_DGRAM; // UDP socket

	int status = getaddrinfo(hostname, SERVERPORTS, &hints, &server_info);
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

	//Binds socket for client
 	if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof my_addr) == -1){
		close(sockfd);
		perror("client: bind failed");
		exit(1);
	}
	//=============================
	//send initial ack message to reciever
	//=============================
	char initM[8];
	memset(initM, 0, 8);
	initialMessage(initM);

	int numbytes = sendto(sockfd, initM, sizeof(initM), 0, ptr->ai_addr, ptr->ai_addrlen);
 	if ((numbytes) == -1)
 	{
 		perror("client: sendto");
 		exit(1);
 	}
	cout << "initial message sent" << endl;

	//=============================
	//seperate message into sizes and begin seqnum
	//=============================
	int sent = 0;		//track current number of messages sent to see when we are done sending
	int messageCount = length/MAXDATALENGTH + 1; //number of messages required to send all of data

	unsigned long seq = 1;			//sequence number
	unsigned char ack = 0x00;		//acknoweldgement
	unsigned char control = 0x00;	//control value

	int remaining_length = length;		//keep track of all the lengths needed
	int message_length = 0;
	int starting = 0;

	char buffer[messageCount][MAXDATALENGTH];

	for (int i = 0; i < (messageCount); i++){				//find out how much to write to the length
		memset(buffer[i],0,MAXDATALENGTH);				//make sure its 0d out
		if (remaining_length > MAXDATALENGTH){
			remaining_length = remaining_length - MAXDATALENGTH;
			message_length = MAXDATALENGTH;
		}
		else{
			message_length = length;
			remaining_length = 0;
		}
		buffer[i][0] = (seq >> 24)& 0xff; //(00000000) 00000000 00000000 00000000
		buffer[i][1] = (seq >> 16)& 0xff; //00000000 (00000000) 00000000 00000000
		buffer[i][2] = (seq >> 8)& 0xff; //00000000 00000000 (00000000) 00000000
		buffer[i][3] = seq & 0xff;		//00000000 00000000 00000000 (00000000)

		buffer[i][4] = ack &0xff; //00000000(00000001) this is the ACK
		buffer[i][5] = control &0xff; //00000000 (00000001) this is the CONTROL

		buffer[i][6] = (message_length >> 8) & 0xff;
		buffer[i][7] = message_length & 0xff;

		for (int x = 0; x < message_length; x++){		//copy message into the string
			buffer[i][8 + x] = message[starting + x];
		}
		starting = starting + message_length;
	}

	//check message
	/*for (int i = 0; i < 27; i++){
		printf("%c \n", buffer[0][i]);
	}*/

	//=============================
 	//Begin listening for incoming messages for a set amount of time
	//=============================
 	struct pollfd pfds[2];

	unsigned long recievedseq;		//get the acks seq num
	unsigned short recievedack;		//get the acks ack value
	unsigned short recievedcontrol;	//get the acks control value
	unsigned short recievedlength;	//get the length of ack

	int initialrecieved = 0;

	//=============================
 	//sEND initialMessage and begin polling
	//=============================


	while (sent < messageCount)
	{
 		struct sockaddr_storage sender_addr;      // sender's address (may be IPv6)
		socklen_t addr1_len = sizeof sender_addr;  // length of this address

		pfds[0].fd = sockfd;
		pfds[0].events = POLLIN;

		pfds[1].fd = 0; //cin
		pfds[1].events = POLLIN;

		int num_events = poll(pfds, 2, 1000);

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

				//get the information from the ack message
				recievedseq = ((recieve[0] << 24) | (recieve[1] << 16) | (recieve[2] << 8) | recieve[3]);
				recievedack = (0 << 8) | recieve[4];
				recievedcontrol = (0 << 8) | recieve[5];
				recievedlength = (recieve[6] << 8) | recieve[7];

				//check to see if it was the initial message or a later one
				if (initialrecieved == 0){
					lastAck = 0;
					int first = 0;
					initialrecieved = 1;
					while (first < 5 && first < messageCount){//send the initial 5 messages to start the
						sent++;
						int numbytes = sendto(sockfd, buffer[first], sizeof(buffer[first]), 0, ptr->ai_addr, ptr->ai_addrlen);
						if ((numbytes) == -1)
					 	{
					 		perror("client: sendto");
					 		exit(1);
					 	}
						first++;
						lastSent = first;
					}

				}
				else if (recievedack == 1 && recievedcontrol == 1){	//if the ack was not the setup ack, add it as received
					tracking[recievedseq] = true;
				}
			}
		}

		//send out the messages that need to be resent or sent for the first time
		for (int i = lastAck; i < lastSent; i++){
			if (tracking[i] == false){		//check to see if the frame between the lastAck and lastSent have been acked
				int numbytes = sendto(sockfd, buffer[i], sizeof(buffer[i]), 0, ptr->ai_addr, ptr->ai_addrlen); //if not resend the message
			 	if ((numbytes) == -1)
			 	{
			 		perror("client: sendto");
			 		exit(1);
			 	}
			}
			/*else if (tracking[i] == true){	//sent for the first time
				if (i - 1 == lastAck)
				  sent++;
					lastAck = i;	//if lastACk has been updated, this means we can send the next message in the array
					int numbytes = sendto(sockfd, buffer[lastAck + 5], sizeof(buffer[lastAck + 5]), 0, ptr->ai_addr, ptr->ai_addrlen); //if not resend the message
				 	if ((numbytes) == -1)
				 	{
				 		perror("client: sendto");
				 		exit(1);
				 	}
					lastSent = i;
			}*/
			//if there are less than the allowed frames out
		}
	}
		printf("hello \n");

	return 0;
}

char* Sender::initialMessage(char* initial){
	unsigned long seqnum = 0;
	unsigned short control = 1;
	unsigned short length = 0;

	initial[0] = (seqnum >> 24)& 0xff; //(00000000) 00000000 00000000 00000000
	initial[1] = (seqnum >> 16)& 0xff; //00000000 (00000000) 00000000 00000000
	initial[2] = (seqnum >> 8)& 0xff; //00000000 00000000 (00000000) 00000000
	initial[3] = seqnum & 0xff;		//00000000 00000000 00000000 (00000000)

	initial[4] = (control >> 8) &0xff; //(00000000) 00000001 this is the ACK
	initial[5] = control &0xff; //00000000 (00000001) this is the CONTROL

	initial[6] = (length >> 8) & 0xff;
	initial[7] = length & 0xff;

	return initial;
}


int Sender::sendMessage(char* buffer, char* sender_ip, char* p, int send){
	return 0;
}

int main (void){

	int size = 20;
	char m[20] = "red1.cs.denison.edu";
	Sender msg(size, m);
	//msg.updateSent(52);

	msg.slidingWindow(m);
	return 0;
}
