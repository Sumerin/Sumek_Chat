#include "Client.h"



bool ipPattern(string IP)
{
	
	integer counter = 0;





	for (integer i = 0; i < 4; i++) // 4 BYTES
	{


		integer number = 0;
		bool blank = true;

		if ((unsigned)counter < IP.size()) // in case there is less then 4
		{

			while (IP[counter] >= '0' && IP[counter] <= '9')// convert char -> int
			{

				number *= 10;
				number += IP[counter] - '0';

				counter++;


				blank = false; // to be sure if there is a number not just "10..0"
			}

			if (blank) // we don't want blank ip
			{
				return false;
			}


			if (IP[counter] != '.' && (unsigned)counter < IP.size()) // dot between Bytes if it isn't last one   
			{

				return false;
			}


			if (number > 255 || number < 0)// BYTE got value <255;0>
			{
				return false;
			}


				counter++; // next sign


		}
		else
		{
			return false;
		}

	}

	return true;
}






#pragma region Private 

bool Client::send_packet(Packet packetType, const char * data, integer size)
{

	integer sentedByte = 0;
	integer sizeBuffer;
	Packet packetTypeBuffer;

	packetTypeBuffer = (Packet)htons((integer)packetType);
	sizeBuffer = htons(size);


	if (size == 0)
	{
		return false; // if some press enter
	}


	/*
	 * package look like 
	 *
	 * [TP]+[SIZE]+[MESSAGE]
	 *
	 * TP- Type of packet in BIG ENDIAN (NETWORK BYTE ORDER)
	 * 
	 * SIZE - size of message also in NBO
	 *
	 * MESSAGE - Vector of CHAR  (no need to convert to NBO )
	 */


	switch (packetType)
	{


	case Packet(Sumek_ChatMessage) :

		


		while (sizeof(Packet)-sentedByte > 0) // TYPE PACKET
		{
			sentedByte += send(server, (char*)&packetTypeBuffer + sentedByte, sizeof(Packet)-sentedByte, NULL);
		}

		sentedByte = 0;

		while (sizeof(integer)-sentedByte > 0) // SIZE
		{
			sentedByte += send(server, (char*)&sizeBuffer + sentedByte, sizeof(integer)-sentedByte, NULL);
		}

		sentedByte = 0;

		while (size - sentedByte > 0)// MESSAGE
		{
			sentedByte += send(server, data + sentedByte, size - sentedByte, NULL);
		}

		break;



	case Packet(Sumek_Command):

		sizeBuffer = htons(size - 1); // don't send '/' example. /me -> me

		while (sizeof(Packet)-sentedByte > 0)// TP
		{
			sentedByte += send(server, (char*)&packetTypeBuffer + sentedByte, sizeof(Packet)-sentedByte, NULL);
		}

		sentedByte = 0;

		while (sizeof(integer)-sentedByte > 0)// SIZE
		{
			sentedByte += send(server, (char*)&sizeBuffer + sentedByte, sizeof(integer)-sentedByte, NULL);
		}

		sentedByte = 0;

		while (size - 1 - sentedByte > 0) // MESSAGE
		{

			sentedByte += send(server, data + 1 + sentedByte, size - 1 - sentedByte, NULL);
		}
		break;


	default:
		return false;
	}
	return true;


}
#pragma endregion

#pragma region Public

#pragma region Get


bool Client::getAlive()
{
	return Alive;
}

#pragma endregion






Client::Client(string server_address,integer port)
{
	

	ServerName = "SERVER:";
	

	sockaddr_in addr;
	integer addrlen = sizeof(addr);
	inet_aton(server_address.c_str(), &addr.sin_addr);

	// out of date addr.sin_addr.s_addr = inet_addr(server_address.c_str());

	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;



	server = socket(AF_INET, SOCK_STREAM, NULL);

	bool isConnected = connect(server, (sockaddr*)&addr, addrlen);


	if ( isConnected)
	{

		exit(0);
	}

	Alive = true;
	if(!fork())
    {
        clientRecvThread();
    }
	

}

Client::~Client()
{

	Alive = false;
	//TODO : WAITING FOR RECV THREAD to closesocket()
}





void Client::clientRecvThread()
{
	Packet packetType;
	Packet packetTypeBuffer;
	integer sizeBuffer;
	integer size;
	integer iResult=1;
	

	while (Alive)
	{
		iResult=recv(server, (char*)&packetTypeBuffer, sizeof(Packet), NULL);

		packetType = (Packet)ntohs((integer)packetTypeBuffer);

		if (iResult <= 0 || !Alive)
		{
            cout<< "ZAMKNIETE";
			Alive = false;
			packetType = Sumek_Close;
		}

		switch (packetType)
		{
			case Packet(Sumek_ChatMessage):
			{
										recv(server, (char*)&sizeBuffer, sizeof(integer), NULL);

										size = ntohs(sizeBuffer);

										char *rBuffer = new char[size + 1];

										recv(server, rBuffer, size, NULL);

										rBuffer[size] = '\0';

										cout << rBuffer << endl;

										delete[] rBuffer;
			}
			break;



			case Packet(Sumek_Command):
			{
										  recv(server, (char*)&sizeBuffer, sizeof(integer), NULL);

										  size = ntohs(sizeBuffer);

										  integer temp = ServerName.size();

										  char *rBuffer = new char[size + 1 + temp ];
										  
										  recv(server, rBuffer + temp, size, NULL);

										  memcpy(rBuffer, ServerName.c_str(), temp);

										  rBuffer[size  + temp] = '\0';


										  cout << rBuffer << endl;
										 

										  delete[] rBuffer;
			}
			break;




			case Packet(Sumek_Close):
			{
										cout << "DISSCONECTED FROM THE SERVER" << endl;
										close(server);
			}
			break;


			default:
				return;
				break;
		}
	}
}


void Client::clientSentMessage(string sBuffer)
{
	
	
	integer size;
	



	size = sBuffer.size();

	switch (sBuffer[0])
	{

	case '/':
		this->send_packet(Packet(Sumek_Command), sBuffer.c_str(), size);
		break;


	default:
		this->send_packet(Packet(Sumek_ChatMessage), sBuffer.c_str(), size);
		break;
	}
	
}

#pragma endregion


