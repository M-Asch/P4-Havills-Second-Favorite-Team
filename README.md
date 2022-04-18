# **CS 375 Project 4**
**Created By:** Riley Collins, Mitch Aschmeyer

This file will explain how to use this sliding window message sending program as well as how it works.
The first section will run through how to use the program while each of the following sections will walk through the sender and receiver functions found in sender.cpp and receiver.cpp.

## **How to Use the Program**
Before you can run the program, you need two open terminals. These terminals can either be running from the same computer or from different machines. Once you have navigated within each terminal to the file directory where these files are held, you are then ready to begin.

Both the protocols for sender and receiver are handled by a file called main.cpp. Once you have compiled main.cpp, you can call to run it in two seperate ways.

    1. Sender: ./a.out {hostname} {message}
    2. Receiver: ./a.out

In the first terminal run main.cpp to start the receiver program. In the second terminal run main.cpp to start the sender protocol with hostname being the host address that the receiver is running on and the message being the message you wish to send. Once both programs have been run, the message sent by the sender will appear in the terminal of the receiver.

## **Sender**

Sender works by first check the number of inputs when the main.cpp exectuable was called. If a hostname and message was included in the call, then the program knows it will be sending the message. It begins by creating a new instance of the Sender class giving it the message along with its size. The sliding window function is then called with the given hostname which will manage the packets and which ones to send or resend. 

**slidingWindow(hostname)**

Sliding window starts by establishing a connection to the given hostname and port. The protocol then creates a starting message which will be used to initialize the connection between the sender and reciever. A buffer is then created which breaks the data up into all of the individual parts which is required if the overall size of the data is greater than 1024. 

```
for (int i = 0; i < (messageCount); i++){		
		memset(buffer[i],0,MAXDATALENGTH + 8);				
		if (remaining_length > MAXDATALENGTH){             
			remaining_length = remaining_length - MAXDATALENGTH;
			message_length = MAXDATALENGTH;
		}
		else{
			message_length = remaining_length;
			remaining_length = 0;
		}
		buffer[i][0] = (htons(seq) >> 24)& 0xff;
		buffer[i][1] = (htons(seq) >> 16)& 0xff; 
		buffer[i][2] = (htons(seq) >> 8)& 0xff; 
		buffer[i][3] = htons(seq) & 0xff;		

		buffer[i][4] = ack &0xff;
		buffer[i][5] = control &0xff; 

		buffer[i][6] = (htons(message_length) >> 8) & 0xff;
		buffer[i][7] = htons(message_length) & 0xff;

		for (int x = 0; x < message_length; x++){
			buffer[i][8 + x] = message[starting + x];
		}
		starting = starting + message_length;
		seq++;
	}
```
This process is used throughout the code to generate messages with sligh variations. *initialMessage()* and *finalMethod* both use versions of this method to create a valid char array with slightly different values and without any actual data present after the header. 

Once these messages are created and sent the function then begins the managment of the messages and acknowledgments. There are a few crucial variables here. *initialrecieved* is used to track if the inital method has been acknowledged and therefor determines if we can begin sending other messages. *acked* tracks the number of acked messages and is used to determine when the program can shut down as all messages have been sent or recieved. 

**Recieving**
The *slidingWindow()* function makes use of the poll function to listen for messages without polling. It also has a timeout feature where if a message is not recieved within a designated amount of time it will end the poll and attempt to resend all messages within the sliding window range that do not have an ACK. (This range can be changed using a constant variable found in "sender.h" called *MAXPACKETSOUT*). 

When a message is recieved the function makes a few checks. It first finds all of the important info such as the sequence number, control, and acknowledgment. 

```
recievedseq = ntohs((recieve[0] << 24) | (recieve[1] << 16) | (recieve[2] << 8) | recieve[3]);
recievedack = (0 << 8) | recieve[4];
recievedcontrol = (0 << 8) | recieve[5];
```

It then checks to see if this is the acknowledgement of the inital message. If it is, the function will then send out as many packets as it can, only limited by either the number of packets in the message or the total number of packets allowed out. It will also update last sent with each new packet it sends in this case as we know this is the first time any of the packets will be sent. 

There are then two more checks which check to see if the message is either the teardown message, in which case it will return and end the function. It will also check to see if this is any other ack rather than the inital or final message. In this case, it takes the sequence number and sets it equal to true in a tracking array. 

**Resending Messages** 

When the *poll()* function ends the code checks to see if it needs to resend a message or if a new message can be sent. 

```
else if(tracking[messageNum] == true){
	if (recievedseq - 2 == lastAck){
	lastAck = recievedseq - 1;
	}
}
```
This checks to see if the lastAck recieved can be updated to allow for a new message to be sent while an else statement will resend the message if it did not recieve an ack for a packet within the given range. 

**Closing**
Finally, once the code has sent all of the messages packets a if statement will be called which continues to send a closing message until an ack is recieved. 

```
else if (acked >= messageCount){
		char finalM[8];
 		memset(finalM, 0, 8);
 		finalMessage(finalM, seq);
 		int numbytes = sendto(sockfd, finalM, sizeof(finalM), 0, ptr->ai_addr, ptr->ai_addrlen);
 		if ((numbytes) == -1){
			perror("client: sendto");
		    	exit(1);
 		}
    	}
	}
```

Once the final ack is recieved the program closes and returns.


## **Receiver**
Once called, the receiver process begins within main.cpp. In main.cpp, the receiver sets up and binds a socket to handle receiving messages from the sender. Following that, main.cpp calls upon the Receiver class and calls the function *recieveMessage*. At this point, the rest of the process is handled within receiver.cpp and the Receiver class. After *recieveMessage*, main.cpp closes the socket and ends the receive process. 

The receiving process within receiver.cpp begins with the function *recieveMessage*. This function starts by receiving the initial message from the sender, saving both the senders IP and Port. Breaking down the initial receive message is handled by the function *initialReceive* which grabs the sequence number, ACK, control value, and length of the message. An if statement then checks to make sure the control value is 1 (or 0x01) to indicate a connection setup, ACK is 0, and that the length is 0 so no extra message is attached to the setup message. If this passes, the function *sendAck* is called to send back an ACK to the sender. If the if statement is not passed, *initialReceive* returns -1, else it returns the sequence number. 

The function *sendAck*, used in the previous function is made to handle any situation to send an ACK to the sender. The function takes in a sequence number, an IP and port, as well as an int that if it equals 1 marks the teardown process. The function establishes a buffer of length 8, determines a control with 0x00 being default, 0x01 for setup, and 0x02 for teardown and then sets up the values for ACK, 0x01, and length 0. The buffer is then set up using bit shifting as follows:

    buffer[0] = (seq >> 24)& 0xff; 
	buffer[1] = (seq >> 16)& 0xff; 
	buffer[2] = (seq >> 8)& 0xff;
	buffer[3] = seq & 0xff;	

	buffer[4] = ack &0xff; 
	buffer[5] = control &0xff;

	buffer[6] = (length >> 8) & 0xff;
	buffer[7] = length & 0xff;

This is followed by setting up a socket to send out the ACK, ending with the function sending out the message to the sender. 

The process then goes back to the *receiveMessage* function where a list of Receiver objects called received is initialized as well as a list on integers called seen. The function moves into a while loop that lasts until the values *LISTENING* is set to something other than 1. The while loop starts the polling process for the bound socket, watching for incoming messages. Once a message is received, the message is broken up to get the sequence number, ACK, control, length, and message data through the following process:

    unsigned long seq1 = receive[0];
    unsigned long seq2 = receive[1];
    unsigned long seq3 = receive[2];
    unsigned long seq4 = receive[3];

    unsigned long seq = ((seq1 << 24) | (seq2 << 16) | (seq3 << 8) | seq4);
    unsigned short ack = (receive[4]);
    unsigned short control = (receive[5]);
    unsigned short length = (receive[6] << 8 | receive[7]);

    char data[length];
    for (int i = 0; i < length; i++){
        data[i] = receive[i + 8];
    }

Once this data is collected, a check is made to see if the sequence number gathered had been seen before and stored in the list seen. If not, the sequence number is added to the list seen and the data for the sequence number, length, and message data is put into a Receiver object and added to the list received. In all situations where control and ACK received are both 0, the function *sendAck* is called to send an ACK back for the sequence number to the sender. If control is 2, an ACK is still sent but LISTENING is set to 0 to exit the loop. 

Once the fuction had left the while loop, it enters the portion of *receiveMessage* that handles assembling and printing the message. First, the number of gathered packets from the while loop is counted into the value *dataLen*. A quick sort, using the function *quickSort* is then is then run on the collected packets to make sure the collected data is in order from lowest to highest sequence number. Afterwards, a loop collects the total of all the lengths stored in each packet stored in the data types in the *received* array. This allows for each amount of data stored in each Receiver object into one char array called *message*. This is done through the following loops:

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

The function *receiveMessage* ends by iterating and using cout to put the message onto the terminal window of receiver, leaving the full message to be displayed.