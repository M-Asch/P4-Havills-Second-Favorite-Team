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

int Receiver::initialReceive(){
  
  
  return 0;
}


int Receiver::waitForMessage(){
  return 0;
}


int Receiver::sendAck(int ack){
  return 0;
}


void Receiver::receiveMessage(int argc, char **argv){
  
  
  
  /*int sockfd;
  struct addrinfo *temp;
  
  if (argc < 2)			//checks that the client inputed a connection to join
	{
		fprintf(stderr, "client: You must specify the server hostname on the command line.\n");
		exit(1);
	}

	struct sigaction sa;		//defines our conditions for our exitprogram to start
	sa.sa_handler = exitProgram;
	sa.sa_flags= 0;
	sigemptyset(&sa.sa_mask);
	if(sigaction (SIGINT, &sa, NULL ) == -1){
		perror("sigaction");
	}

	//=======================================================
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

	//Sends entered username to server
 	int numbytes = sendto(sockfd, argv[2], strlen(argv[2]), 0, ptr->ai_addr, ptr->ai_addrlen);
 	if ((numbytes) == -1)
 	{
 		perror("client: sendto");
 		exit(1);
 	}

	//Sets up some variables we will use for later
	char chatFriend[MAXBUFLEN];
	int stage = 0;
	//int LISTENING = 1;


	while (stage < 2)
	{
			char buffer[MAXBUFLEN]; //buffer to hold recieved message
			struct sockaddr_storage sender_addr;      // sender's address (may be IPv6)
			socklen_t addr1_len = sizeof sender_addr;  // length of this address

			if (stage == 0){ // Stage 1: Initial chatroom set up
					//Accepts message from server
					int numbytes = recvfrom(sockfd, buffer, MAXBUFLEN - 1 , 0, (struct sockaddr *)&sender_addr, &addr1_len);
					if (numbytes == -1)
					{
						perror("recvfrom");
						exit(1);
					}

					buffer[numbytes] = '\0';
					printf("\"%s\"\n", buffer);

					if (strcmp(buffer, "Connected! (Type 'EXIT' for your message to end the chat).") == 0) // Indicates that two clients are connected and chat can move to next stage
					{
							stage++;
					}
			}
			else if (stage == 1){ // Stage 2: Chat Partner usernames
					int numbytes = recvfrom(sockfd, buffer, MAXBUFLEN - 1 , 0, (struct sockaddr *)&sender_addr, &addr1_len);
					if (numbytes == -1)
					{
						perror("recvfrom");
						exit(1);
					}

					buffer[numbytes] = '\0';
					strcpy(chatFriend, buffer);
					printf("%s\n", chatFriend);
					stage++;
			}
	}

	// POLLING STUFF
	struct pollfd pfds[2];

	while (LISTENING)
	{
		char buffer[MAXBUFLEN]; //buffer to hold recieved message
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
			int pollin_happened2 = pfds[1].revents & POLLIN;

			if (pollin_happened1){
					//Stage 3: Chat
					char recieve[MAXBUFLEN];
        	int numbytes = recvfrom(sockfd, recieve, MAXBUFLEN - 1 , 0, (struct sockaddr *)&sender_addr, &addr1_len);
		    	if (numbytes == -1)
		    	{
			     		perror("recvfrom");
			     		exit(1);
		   		}

					recieve[numbytes] = '\0';
					if (strcmp(recieve, "EXIT") == 0) // Checks if chat partner has sent exit command, if so will prompt end of chat
		   		{
		   			printf("%s has disconnected.\n", chatFriend);
		   			LISTENING = 0;
	    		}
	    		else // Will print the message your chat partner sent in the form 'chatFriend: "MESSAGE"'
	    		{
		    			printf("%s: \"%s\"\n", chatFriend, recieve);
		    	}

			} else if (pollin_happened2) {
				char message[MAXBUFLEN];

				fgets(message, MAXBUFLEN, stdin);
				int size = strlen(message);
				message[size-1] = '\0';

				if (strcmp(message, "EXIT") == 0) //Checks if exit command gets typed to prompt closing chat
				{
						LISTENING = 0;
				}

				// Sends typed message to chat partner through server
				int numbytes = sendto(sockfd, message, strlen(message), 0, ptr->ai_addr, ptr->ai_addrlen);
				if ((numbytes) == -1)
				{
						perror("client: sendto");
						exit(1);
				}
			} 
		}
	}

	freeaddrinfo(server_info);

 	close(sockfd);*/

}
