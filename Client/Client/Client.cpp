#include "Client.h"

bool WSAData_run = false;// be sure not to start wsData twice


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




#pragma region ThreadCreate
void startRecv(void *arg) // Create_thread and start reciving infromation from serwer
{
	Client * temp = (Client *)arg;
	temp->clientRecvThread();
	
}

#pragma endregion 


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


	case Packet::Sumek_ChatMessage:

		


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



	case Packet::Sumek_Command:

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

		/* no need after i found out closesocket();
	case Packet::Sumek_Close:

		while (sizeof(Packet)-sentedByte > 0) // JUST PACKET TYPE
		{
			sentedByte += send(server, (char*)&packetTypeBuffer + sentedByte, sizeof(Packet)-sentedByte, NULL);
		}

		break;
		*/
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
	
	start_WSADATA();
	

	SOCKADDR_IN addr;
	integer addrlen = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr(server_address.c_str());
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;



	server = socket(AF_INET, SOCK_STREAM, NULL);

	if (connect(server, (SOCKADDR*)&addr, addrlen) != 0)
	{
		MessageBoxA(NULL, "Failed to connect", "Sumek_Chat_ERROR", MB_OK || MB_ICONERROR);
		exit(0);
	}

	Alive = true;
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)startRecv, (LPVOID) this, NULL, NULL);
	

}

Client::~Client()
{

	Alive = false;
	//TODO : WAITING FOR RECV THREAD to closesocket()
}



void Client::start_WSADATA()
{
	if (WSAData_run == false)
	{

		WSADATA wsaData;
		WORD dllVersion = MAKEWORD(2, 1);

		if (WSAStartup(dllVersion, &wsaData) != 0)
		{
			MessageBoxA(NULL, "Startup Failed", "Sumer_chat_Error", MB_OK | MB_ICONERROR);
			exit(1);
		}
	}
	WSAData_run = true;
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

		if (iResult < 0 || !Alive)
		{
			Alive = false;
			packetType = Packet::Sumek_Close;
		}

		switch (packetType)
		{
			case Packet::Sumek_ChatMessage:
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



			case Packet::Sumek_Command:
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




			case Packet::Sumek_Close:
			{
										cout << "DISSCONECTED FROM THE SERVER" << endl;
										closesocket(server);
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
		this->send_packet(Packet::Sumek_Command, sBuffer.c_str(), size);
		break;


	default:
		this->send_packet(Packet::Sumek_ChatMessage, sBuffer.c_str(), size);
		break;
	}
	
}

#pragma endregion


