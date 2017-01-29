#ifndef SERVER_H
#define SERVER_H
#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>
#include <iostream>
#include <cstdint>
#include <string>

#define MAX_SIZE 10 // max user number per ROOM

typedef int32_t integer; // make sure to have integer on 4 Bytes

using namespace std; // for String and cout

enum class Packet : integer // typ of packet to send integer =>4 Bytes
{
	Sumek_ChatMessage,
	Sumek_Command,
	Sumek_Close
};

class Room;

struct member{ // to storage the data about the other side
	SOCKET Connection;
	bool Alive = false; 
	string nickname = "Adam";// new guy is always Adam
};



struct pass_arg{ // just passing arguments for thread
	Room *dest;
	integer number;
};



class Room
{
private:


	


	integer RoomIndex; // invidual room number currently not in use

	member client[MAX_SIZE];// stored information


	integer numberOfClient = 0; // counter of used slot

	string MOTD = "Welcome! Beta_Test Room";
	string Reject_1 = "Sorry! Too many Clients";


	bool send_packet(Packet packetType, const char * data, integer size, const SOCKET dest); // this boolean is still useless always sented 

public:
	Room(integer number);

	static void start_WSADATA();// Winsock on windows


	void start();// useless

	void join(SOCKET newConnetcion); // join client to current Room
	


	void income_msg(integer num, pass_arg* relase); // start_thread use it. income message service for user number = num
	
};






#endif