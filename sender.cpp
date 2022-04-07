#include <iostream>
#include "sender.h"

using namespace std;

Sender::Sender(){
	message = [1024];
	length = 1024;
	lastSent = 0;
	lastAck = 0;
}

int main (void){

	return 0;
}
