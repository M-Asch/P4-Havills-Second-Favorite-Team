#include <iostream>

#include <stdio.h> //printf
#include <stdlib.h> //exit
#include <unistd.h> //close
#include <string.h> //memset
#include <arpa/inet.h> //inet_ntop
#include <netdb.h> //sender
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <poll.h>

//===========================
//		  Definitions 
//
//===========================

#define SERVERPORT 9090
#define SERVERPORTS "9090"
#define MAXBUFLEN 100

using namespace std;

class Sender{

	public:
		Sender(int size, char *msg);
	
		struct sockaddr_in makeSocket(void);
		void setMessage(int size, char *msg);
		void updateSent(int newlySent);
		void updateAck(int newAck);
		
		int getSent();
		int getAck();
		char* getMessage();
	
		int slidingWindow(char* hostname);
		char* initialMessage(char* initial);
		int sendMessage(char* buffer, char* sender_ip, char* p, int send);
	
	private:
		char* message;
		bool* buffer;
		int length;
		int lastSent;
		int lastAck;
};
