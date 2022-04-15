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
    Receiver(int s, int l, char* c);
    int getSeq();
    int getLen();
    char* getMessage();
    void setSeq(int s);
    void setLen(int l);
    void setMessage(char* m);

    int initialReceive(char buffer[], char* sender_ip, char* p);
    int sendAck(unsigned long seq, char* sender_ip, char* p, int final);
    void receiveMessage(int sockfd);

    void quickSort(Receiver arr[], int start, int end);
    int partition(Receiver arr[], int start, int end);
    void swap(Receiver *a, Receiver *b);

  private:
    int seqnum;
    int len;
    char* message;
};
