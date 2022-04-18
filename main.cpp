#include <iostream>
#include <fstream>

#include "sender.cpp"
#include "receiver.h"

using namespace std;

int main(int argc,char* argv[]){
  /*
  In order to use as sender use ./a.out {hostname} {message}
  */
	if(argc == 3){//This instance is a sender
 
		int size = 0;    
    while (argv[2][size] != 0){    //get size of message and creat instance
      size++;
    }
    char m[size];
    for (int i = 0; i < size; i++){
      m[i] = argv[2][i];
    }
  	Sender msg(size, m);    
  
    int sizeHost = 1;
    while (argv[1][sizeHost] != 0){      //get hostname and size of hostname
      sizeHost++;
    }
    
    char host[sizeHost];
    for (int i = 0; i < 20; i++){  
      host[i] = argv[1][i];    
    }
  	msg.slidingWindow(host);      
  	return 0;
  }
	return 0;
}
