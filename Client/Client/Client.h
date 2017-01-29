#ifndef CLIENT_H
#define CLIENT_H
#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>
#include <cstdint>
#include <iostream>
#include <string>


using namespace std;

typedef int32_t integer; // integer is 4 Bytes for Sure



enum class Packet : integer // Type os pacekt to be send 4 Bytes also 
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

	string ServerName = "SERVER:"; // temporary until i decide if it's constat or changable

	
	
	
	bool send_packet(Packet packetType, const char * data, integer size);

public:

	Client(string server_address, integer port);
	~Client();



	static void start_WSADATA(); // windows winsock



	void close();



	void clientRecvThread(); // start recv message from server on another thread

	void clientSentMessage(); // sent message to the server

	


	

};



#endif

