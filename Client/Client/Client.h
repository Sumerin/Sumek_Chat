#ifndef CLIENT_H
#define CLIENT_H


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

typedef int SOCKET ;

#include <unistd.h>
#include <stdlib.h>

#include <iostream>
#include <string.h>


using namespace std;

typedef int32_t integer; // integer is 4 Bytes for Sure



enum  Packet  // Type os pacekt to be send 4 Bytes also
{
	Sumek_ChatMessage,
	Sumek_Command,
	Sumek_Close
};
 


class Client
{

private:

	SOCKET server;

	bool Alive;

	string ServerName ; // temporary until i decide if it's constat or changable

	
	
	
	bool send_packet(Packet packetType, const char * data, integer size);

public:

	bool getAlive();




	Client(string server_address, integer port);
	~Client();








	void clientRecvThread(); // start recv message from server on another thread

	void clientSentMessage(string Text); // sent message to the server

	


	

};





bool ipPattern(string);



#endif

