// receiver.h

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
#include <algorithm>


//===========================
//		  Definitions
//
//===========================

#define SERVERPORT 9090
#define SERVERPORTS "9090"
#define MAXBUFLEN 1034

using namespace std;

class Receiver{

  public:
    Receiver();
    Receiver(int s, char* c);
    int getSeq();
    char* getMessage();
    void setSeq(int s);
    void setMessage(char* m);

    int initialReceive(char buffer[], int sockfd, struct addrinfo *ptr); //char ip[], char port[]
    int sendAck(unsigned long seq, int sockfd, struct addrinfo *ptr); //char ip[], char port[]
    void receiveMessage(int sockfd, struct addrinfo *ptr); //int argc, char **argv

  private:
    int seqnum;
    char* message;
};
