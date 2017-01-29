#include "Client.h"

bool WSAData_run = false;// be sure not to start wsData twice

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

	switch (packetType)
	{
	case Packet::Sumek_ChatMessage:


		while (sizeof(Packet)-sentedByte > 0)
		{
			sentedByte += send(server, (char*)&packetTypeBuffer + sentedByte, sizeof(Packet)-sentedByte, NULL);
		}

		sentedByte = 0;

		while (sizeof(integer)-sentedByte > 0)
		{
			sentedByte += send(server, (char*)&sizeBuffer + sentedByte, sizeof(integer)-sentedByte, NULL);
		}

		sentedByte = 0;

		while (size - sentedByte > 0)
		{
			sentedByte += send(server, data + sentedByte, size - sentedByte, NULL);
		}

		break;



	case Packet::Sumek_Command:

		sizeBuffer = htons(size - 1); // nie wysylamy '/' np. /me -> me

		while (sizeof(Packet)-sentedByte > 0)
		{
			sentedByte += send(server, (char*)&packetTypeBuffer + sentedByte, sizeof(Packet)-sentedByte, NULL);
		}

		sentedByte = 0;

		while (sizeof(integer)-sentedByte > 0)
		{
			sentedByte += send(server, (char*)&sizeBuffer + sentedByte, sizeof(integer)-sentedByte, NULL);
		}

		sentedByte = 0;

		while (size - 1 - sentedByte > 0)
		{

			sentedByte += send(server, data + 1 + sentedByte, size - 1 - sentedByte, NULL);
		}
		break;

	case Packet::Sumek_Close:

		while (sizeof(Packet)-sentedByte > 0)
		{
			sentedByte += send(server, (char*)&packetTypeBuffer + sentedByte, sizeof(Packet)-sentedByte, NULL);
		}

		break;

	default:
		return false;
	}
	return true;


}
#pragma endregion

#pragma region Public
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
	send_packet(Packet::Sumek_Close, nullptr, NULL);
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

		if (iResult <= 0)
		{
			delete this;
			return;
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
			default:
				return;
				break;
		}
	}
}


void Client::clientSentMessage()
{
	string sBuffer;
	string bufferBuffera;
	integer size;
	

	getline(cin, sBuffer);

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

