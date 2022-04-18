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

## **sender.cpp**

## **receiver.cpp**
