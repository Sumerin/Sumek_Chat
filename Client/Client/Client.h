#ifndef CLIENT_H
#define CLIENT_H


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>


using namespace std;

typedef int32_t integer; // integer is 4 Bytes for Sure

typedef int SOCKET ;


enum  Packet  // Type  sending/reciving message
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

	    string ServerName ;

	    bool send_packet(Packet packetType, const char * data, integer size);

    public:

	    Client(string server_address, integer port);
	    ~Client();

        void clientRecvThread(); // start recv message from server on another thread

	    void clientSentMessage(string Text); // sent message to the server
	

};





bool ipPattern(string);

void check_errno(integer err);       // checking errno and switching off client


#endif

