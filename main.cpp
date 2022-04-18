#include <iostream>
#include <fstream>

#include "sender.cpp"
#include "receiver.cpp"

using namespace std;


//===============================================
//               makeSocket
//     create socket for receiving info
//===============================================
struct sockaddr_in makeSocket(void){
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVERPORT);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	memset(server_addr.sin_zero, 0, sizeof server_addr.sin_zero);

	return server_addr;
}


int main(int argc,char* argv[]){
  /*
  In order to use as sender use ./a.out {hostname} {message}

  In order to use as receiver use ./a.out
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
  else{
    // ESTABLISH SOCKET
    int sockfd = socket(PF_INET, SOCK_DGRAM, 0);    // create a new UDP socket
    if (sockfd == -1) {
      perror("server: socket failed");
      exit(0);
    }

    struct sockaddr_in server_addr = makeSocket();	//fill in socket ids
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof server_addr) == -1){
      close(sockfd);
      perror("server: bind failed");
      exit(1);
    }

	  // ALL THE RECEIVING AND CHECKING IS HANDLED THROUGH THIS FUNCTION
 	  Receiver r;
	  r.receiveMessage(sockfd);
 	  close(sockfd);

    return 0;
  }
	return 0;
}
