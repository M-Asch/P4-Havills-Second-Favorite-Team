// receiver.cpp

#include <iostream>
#include "receiver.h"

using namespace std;

//==========================================
//               Receiver
//         Initializes a Receiver
//==========================================
Receiver::Receiver(){
  seqnum = 0;
  len = 0;
  message = new char[1024];
}

//==========================================
//               Receiver
//  Initializes a Receiver with inputed values
//==========================================
Receiver::Receiver(int s, int l, char* c){
  seqnum = s;
  len = l;
  message = new char[1024];
  for (int i = 0; i < l; i++){
    message[i] = c[i];
  }
}

//==========================================
//               getSeq
//           returns seqnum
//==========================================
int Receiver::getSeq(){
  return seqnum;
}

//==========================================
//               getLen
//           returns length
//==========================================
int Receiver::getLen(){
  return len;
}

//==========================================
//               getMessage
//           returns message
//==========================================
char* Receiver::getMessage(){
  return message;
}

//==========================================
//               setSeq
//        sets seqnum to inputed value
//==========================================
void Receiver::setSeq(int s){
  seqnum = s;
}

//==========================================
//               setLen
//        sets len to inputed value
//==========================================
void Receiver::setLen(int l){
  len = l;
}

//==========================================
//              setMessage
//    sets message as inputed char array
//==========================================
void Receiver::setMessage(char* m){
  message = m;
}

//====================================================
//               initialReceive
//      Handles the process for the connection setup
//====================================================
int Receiver::initialReceive(char buffer[], char* sender_ip, char* p){
    // Set up sequence number for bit shifting
    unsigned long seq1 = buffer[0];
    unsigned long seq2 = buffer[1];
    unsigned long seq3 = buffer[2];
    unsigned long seq4 = buffer[3];
    // bit shift buffer to place chars into proper datatypes
    unsigned long seq = ((seq1 << 24) | (seq2 << 16) | (seq3 << 8) | seq4);
    unsigned short ack = (buffer[4]);
    unsigned short control = (buffer[5]);
    unsigned short length = (buffer[6] << 8 | buffer[7]);

    //check that values for setup messages match, if yes send acknowledgement
    if(control == 1 && ack == 0 && length == 0){
      sendAck(seq, sender_ip, p, 0);
    }
    else{
      return -1;
    }

  	return seq;
}

//=================================================================
//                          sendAck
//      sends ACK back to the sender for data seq num recieved
//=================================================================
int Receiver::sendAck(unsigned long seq, char* sender_ip, char* p, int final){
  char buffer[8];
  memset(buffer, 0, 8);

  // Establishes control value in buffer, data = 0, setup = 1, teardown = 2
	unsigned char control = 0x00;
  if (seq == 0){
    control = 0x01;
  }
  else if (final == 1){
    control = 0x02;
  }
  unsigned char ack = 0x01;
	unsigned short length = 0;

  // bit shifting for values into buffer
	buffer[0] = (seq >> 24)& 0xff; //(00000000) 00000000 00000000 00000000
	buffer[1] = (seq >> 16)& 0xff; //00000000 (00000000) 00000000 00000000
	buffer[2] = (seq >> 8)& 0xff; //00000000 00000000 (00000000) 00000000
	buffer[3] = seq & 0xff;		//00000000 00000000 00000000 (00000000)

	buffer[4] = ack &0xff; //(00000000) 00000001 this is the ACK
	buffer[5] = control &0xff; //00000000 (00000001) this is the CONTROL

	buffer[6] = (length >> 8) & 0xff;
	buffer[7] = length & 0xff;


  // Setting up address information to send ACK
  struct addrinfo hints, *server_info;

 	memset(&hints, 0, sizeof(hints));
 	hints.ai_family = AF_INET; //IPv4
 	hints.ai_socktype = SOCK_DGRAM; // UDP socket
  //cout << "seq " << seq << endl;
 	int status = getaddrinfo(sender_ip, p, &hints, &server_info);
 	if (status != 0)
 	{
 		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
 		exit(1);
 	}

 	int newSock;	//newsocket to send info to
 	struct addrinfo *ptr = server_info; //ptr to current struct addrinfo
 	while (ptr != NULL){				//attempt to build a socket for the senders ip and port
 		newSock = socket(ptr->ai_family, ptr->ai_socktype, 0);
 		if (newSock != -1){
 			break;
 		}
 		ptr = ptr->ai_next;
 	}
 	if (ptr == NULL){
 		fprintf(stderr, "client: failed to create socket\n");
 		exit(2);
 	}

  // Send the ACK buffer to the sender
  int numbytes = sendto(newSock, buffer, sizeof(buffer), 0, ptr->ai_addr, ptr->ai_addrlen);
 	if ((numbytes) == -1)
 	{
 		return -1; // shows error occured in the sendto
 	}

  return 0;
}

//====================================================
//               quickSort
//      Sorts an array from least to greatest
//====================================================
void Receiver::quickSort(Receiver arr[], int low, int high){
  if (low < high){
    int p = partition(arr, low, high);
    quickSort(arr, low, p-1);
    quickSort(arr, p+1, high);
  }
}

//====================================================
//               partition
//      partition of array for quickSort
//====================================================
int Receiver::partition(Receiver* arr, int low, int high){
  int pivot = arr[high].getSeq();
  int i = (low-1);

  for (int j = low; j < high; j++){
    if (arr[j].getSeq() <= pivot){
      i++;

      swap(&arr[i], &arr[j]);
    }
  }

  swap(&arr[i+1], &arr[high]);

  return (i+1);
}

//====================================================
//               swap
//      swaps parts of an array of Receivers
//====================================================
void Receiver::swap(Receiver *a, Receiver *b){
  Receiver t = *a;
  *a = *b;
  *b = t;
}


//=================================================================================
//                                 receiveMessage
//      Handles the receiving messages and keeping track of what has been seen
//=================================================================================
void Receiver::receiveMessage(int sockfd){
  char ip[16];
	char portC[6];
	int LISTENING = 1;

	//Prepare to recieve message
	char buffer[MAXBUFLEN];                   // buffer to hold received message
	struct sockaddr_storage sender_addr;      // sender's address (may be IPv6)
	socklen_t addr1_len = sizeof sender_addr;  // length of this address

	// wait for a message
	int numbytes = recvfrom(sockfd, buffer, MAXBUFLEN - 1 , 0, (struct sockaddr *)&sender_addr, &addr1_len);//receive message information
	if (numbytes == -1)
	{
		perror("recvfrom");
		exit(1);
	}

	//Get info about sender socket
	char sender_ip_string[INET6_ADDRSTRLEN];
	inet_ntop(sender_addr.ss_family, &(((struct sockaddr_in*)&sender_addr)->sin_addr), sender_ip_string, INET6_ADDRSTRLEN);	//recieve info about sender

	struct sockaddr_in *s = (struct sockaddr_in *)&sender_addr;			//sockaddr_in storage for the sender socket
	int port = ntohs(s->sin_port);							//this is used to get the port
	char p[6];			//used to store the port string

	sprintf(p, "%d", port);					//turn port into string

	buffer[numbytes] = '\0'; // add null terminator for printing (finalize message)
	//check to see if clients need to be updated, fill in client information
	memcpy(ip, sender_ip_string, sizeof(sender_ip_string));			//save ip
	memcpy(portC, p, sizeof(p));		//and port string

  // Handles initial setup message
  if(initialReceive(buffer, ip, portC) == -1){
    perror("initialReceive");
    exit(1);
  }

  // sets up list to store received data
  Receiver received[MAXBUFLEN] = {};
  memset(received, 0, MAXBUFLEN);
  int seen[MAXBUFLEN] = {};
  memset(seen, 0, MAXBUFLEN);
  int count = 0;

  struct pollfd pfds[1];

  // Handles rest of receiving data
	while (LISTENING == 1){
    char buffer[MAXBUFLEN]; //buffer to hold recieved message
 		struct sockaddr_storage sender_addr;      // sender's address (may be IPv6)
		socklen_t addr1_len = sizeof sender_addr;  // length of this address

		pfds[0].fd = sockfd;
		pfds[0].events = POLLIN;

		int num_events = poll(pfds, 2, -1);

		if (num_events != 0) {
			//printf("Type message: "); //Prompts client to enter message to send to chat partner
			int pollin_happened = pfds[0].revents & POLLIN;
      if (pollin_happened){
          char receive[MAXBUFLEN]; //receives incoming buffer
          int numbytes = recvfrom(sockfd, receive, MAXBUFLEN - 1, 0, (struct sockaddr *)&sender_addr, &addr1_len);
          if (numbytes == -1){
			     		perror("recvfrom");
			     		exit(1);
		   		}

          // breaks up buffer into proper datatypes
          unsigned long seq1 = receive[0];
          unsigned long seq2 = receive[1];
          unsigned long seq3 = receive[2];
          unsigned long seq4 = receive[3];

          unsigned long seq = ((seq1 << 24) | (seq2 << 16) | (seq3 << 8) | seq4);
          //cout << "seq " << seq << endl;
          unsigned short ack = (receive[4]);
          //cout << "ack " << ack << endl;
          unsigned short control = (receive[5]);
          //cout << "control " << control << endl;
          unsigned short length = (receive[6] << 8 | receive[7]);
          //cout << "len " << length << endl;
          char data[length];
          for (int i = 0; i < length; i++){
             data[i] = receive[i + 8];
             //cout << data[i] << endl;
          }

          // checks to see if seqnum had been seen yet, if not adds to lists to store data and keep track of what has been seen
          bool temp = (std::find(std::begin(seen), std::end(seen), seq) != std::end(seen));
          if (temp == false && control != 2){
             //cout << "seen " << seq << endl;
             seen[count] = seq;
             Receiver add(seq, length, data);
             received[count] = add;
             count++;
          }

          // checks variables that should not have changed to allow for ACK to be sent
          if(control == 0 && ack == 0){
            sendAck(seq, ip, portC, 0);
          }
          else if(control == 2 && ack == 0){ // Handles sending the ACK for teardown
            LISTENING = 0;
            sendAck(seq, ip, portC, 1);
          }
	    }
    }
  }
  cout << "out of loop" << endl;

  //Sort packets into order
  int dataLen = 0;
  bool t = true;
  while (t == true){
    if (received[dataLen].getSeq() == 0){
      t = false;
      dataLen--;
    }
    dataLen++;
  }

  // Sort out out of order sequence numbers
  quickSort(received, 0, (dataLen-1));

  // gets total length of all chars received
  int totalLen = 0;
  for (int i = 0; i < dataLen; i++){
     totalLen = totalLen + received[i].getLen();
  }

  // pieces together the original message and stores in message
  char message[totalLen] = {};
  int c = 0;
  while (c < totalLen){
    for (int i = 0; i < dataLen; i++){
      char* temp = NULL;
      temp = received[i].getMessage();
      for (int j = 0; j < received[i].getLen(); j++){
        message[c] = temp[j];
        c++;
      }
    }
  }

  //returns message in receiver terminal
  for (int i = 0; i < totalLen; i++){
    cout << message[i];
  }
  cout << endl;

  //message[totalLen] = '\0';
  //printf("\"%s\"\n", message);
}


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

//===============================================
//               main
//     sets up socket and calls receiveMessage
//===============================================
int main(int argc, char **argv){

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
  cout << "start receiving" << endl;
	r.receiveMessage(sockfd);
  cout << "end receive" << endl;
  //freeaddrinfo(server_info);
 	close(sockfd);

  return 0;
}
