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
    int getLastRecieve();
    int getLastAcceptable();
    int* getSeen();
    void setSeen();

    int initialReceive(char buffer[], int sockfd, struct addrinfo *ptr); //char ip[], char port[]
    int sendAck(unsigned long seq, int sockfd, struct addrinfo *ptr); //char ip[], char port[]
    void receiveMessage(int sockfd, struct addrinfo *ptr); //int argc, char **argv

  private:
    int lastRecieve;
    int lastAcceptable;
    int* seen;
};
