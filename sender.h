#include <iostream>

using namespace std;

class Sender{

	public:
		Sender();
		Sender(int size);
		Sender(int size, char *msg);
	
		void setMessage(int size, char *msg);
	
		int slidingWindow();
		int initialMessage();
		int sendMessage();
	
	private:
		char *message;
		int length;
		int lastSent;
		int lastAck;
};
