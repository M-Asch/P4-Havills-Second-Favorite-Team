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
	
		void setMessage(int size, char *msg);
	
		int slidingWindow();
		int initialMessage();
		void sendMessage(char* buffer, char* sender_ip, char* p);
	
	private:
		char* message;
		int length;
		int lastSent;
		int lastAck;
};
