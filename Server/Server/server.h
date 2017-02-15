#ifndef SERVER_H
#define SERVER_H


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

#define MAX_SIZE 10 // max user number per ROOM

typedef int32_t integer; // make sure to have integer on 4 Bytes

using namespace std; // for String and cout

enum  Packet  // typ of packet to send integer =>4 Bytes
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




	void join(SOCKET newConnetcion); // join client to current Room
	


	void income_msg(integer num); // start_thread use it. income message service for user number = num
	
};






#endif