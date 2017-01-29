#include "server.h"

bool WSAData_run = false; // be sure not to start wsData twice

	bool Room::send_packet(Packet packetType, const char * data, integer size, const SOCKET dest)
	{


	integer sizeBuffer;
	Packet packetTypeBuffer;

	sizeBuffer = htons(size);
	packetTypeBuffer = (Packet)htons((integer)packetType);

	switch (packetType)
	{
	case Packet::Sumek_ChatMessage:

		send(dest, (char*)&packetTypeBuffer, sizeof(Packet), NULL);
		send(dest, (char*)&sizeBuffer, sizeof(integer), NULL);
		send(dest, data, size, NULL);
		break;
	case Packet::Sumek_Command:

		break;

	default:
		break;
	}
	return true;


	}


	void  start_thread(void * arg) // function used with create_thread to make new thread for another client
	{
		pass_arg *temp = (pass_arg*)arg;
		temp->dest->income_msg(temp->number, temp);
	}


	Room::Room(integer number)
	{
		start_WSADATA();
		RoomIndex = number;
	}

	void Room::start_WSADATA()
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

	void start()
	{
		//TODO: thread obslugujacy wszystkie wiadomsci przychodzace do roomu 
	}

	void Room::join(SOCKET newConnetcion)
	{

		integer size = MOTD.size();
		pass_arg *arg = new pass_arg();

		if (numberOfClient == MAX_SIZE)
		{
			size = Reject_1.size();
			send_packet(Packet::Sumek_ChatMessage, Reject_1.c_str(), size, newConnetcion);
			return;
		}

		client[numberOfClient].Connection = newConnetcion;
		client[numberOfClient].Alive = true;

		arg->dest = this;
		arg->number = numberOfClient++;

		send_packet(Packet::Sumek_ChatMessage, MOTD.c_str(), size, newConnetcion);

		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)start_thread, (LPVOID)arg, NULL, NULL);
	}


	void Room::income_msg(integer num, pass_arg* relase)
	{
		delete relase;
		int iResult = 1;
		while (client[num].Alive)
		{
			char *buffer;
			integer sizeBuffer;
			integer size;
			integer nicknameSize;
			Packet packetType;
			Packet packetTypeBuffer;


			iResult = recv(client[num].Connection, (char *)&packetTypeBuffer, sizeof(Packet), NULL);

			packetType = (Packet)ntohs((integer)packetTypeBuffer);

			if (iResult <= 0)
			{
				packetType = Packet::Sumek_Close;
			}



			switch (packetType)
			{


				case Packet::Sumek_ChatMessage:
				{
											  recv(client[num].Connection, (char *)&sizeBuffer, sizeof(integer), NULL);

											  size = ntohs(sizeBuffer);
											  nicknameSize = client[num].nickname.size();
											  size += nicknameSize + 2;
											  buffer = new char[size + 1];

											  memcpy(buffer, client[num].nickname.c_str(), nicknameSize);

											  buffer[nicknameSize] = ':';
											  buffer[nicknameSize + 1] = ' ';

											  recv(client[num].Connection, buffer + nicknameSize + 2, size, NULL);
											  buffer[size] = '\0';
											  cout << num << ": " << buffer << std::endl;

											  for (int i = numberOfClient - 1; i >= 0; i--)
											  {
												  if (i == num || !client[i].Alive)
												  {
													  continue;
												  }

												  send_packet(Packet::Sumek_ChatMessage, buffer, size, client[i].Connection);
											  }

											  delete buffer;
				}
				break;


				case Packet::Sumek_Command:
				{
										 recv(client[num].Connection, (char *)&sizeBuffer, sizeof(integer), NULL);

										  size = ntohs(sizeBuffer);

										  buffer = new char[size];

										  recv(client[num].Connection, buffer, size, NULL);



										  delete[] buffer;
				}
				break;


				case Packet::Sumek_Close:
				{
											client[num].Alive = false;
											nicknameSize = client[num].nickname.size();
											size = nicknameSize + sizeof(": Disconected!!");
											buffer = new char[size + 1];

											memcpy(buffer, client[num].nickname.c_str(), nicknameSize);
											memcpy(buffer + nicknameSize, ": Disconected!!", sizeof(": Disconected!!"));



											buffer[size] = '\0';
											cout << num << ": " << buffer << std::endl;

											for (int i = numberOfClient - 1; i >= 0; i--)
											{
												if (i == num || !client[i].Alive)
												{
													continue;
												}

												send_packet(Packet::Sumek_ChatMessage, buffer, size, client[i].Connection);
											}

											delete[] buffer;
				}
				break;
			}
			
		}
	}






