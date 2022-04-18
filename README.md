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