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
    int getSeen();
    void setSeen();

    int initialReceive(char buffer[], char ip[], char port[]);
    //int waitForMessage();
    int sendAck(unsigned long seg, char ip[], char port[]);

    void receiveMessage(int argc, char **argv);

  private:
    int lastRecieve;
    int lastAcceptable;
    int seen[1024];
};
